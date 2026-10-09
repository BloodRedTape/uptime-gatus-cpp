#include "gatus/provider/worker_provider.hpp"
#include <stdexcept>
#include <utility>

namespace {
std::string EncodePathSegment(const std::string& value) {
    const char* hex = "0123456789ABCDEF";
    std::string result;
    for (unsigned char ch : value) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            result += static_cast<char>(ch);
        } else {
            result += '%';
            result += hex[ch >> 4];
            result += hex[ch & 15];
        }
    }
    return result;
}
}

Gatus::WorkerProvider::WorkerProvider(std::unique_ptr<HttpClient>&& client,
    const std::string& api_url, const std::string& endpoint_key)
    : m_Client(std::move(client)), m_ApiUrl(api_url),
      m_PushPath("/api/v1/endpoints/" + EncodePathSegment(endpoint_key) + "/external?success=true") {
    if (!m_Client || api_url.empty() || endpoint_key.empty())
        throw std::invalid_argument("Gatus client, URL and endpoint key are required");
    m_WorkerThread = std::thread(&WorkerProvider::WorkerLoop, this);
}

Gatus::WorkerProvider::~WorkerProvider() {
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_StopFlag = true;
    }
    m_Condition.notify_all();
    if (m_WorkerThread.joinable()) m_WorkerThread.join();
}

void Gatus::WorkerProvider::Push(const std::string& token) {
    if (token.empty() || token.find_first_of("\r\n") != std::string::npos) {
        Log(Verbosity::Error, "Invalid Gatus token; push skipped.");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_StopFlag) return;
        m_PushTokens.push_back(token);
    }
    m_Condition.notify_one();
}

bool Gatus::WorkerProvider::AnyPending() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return !m_PushTokens.empty();
}

void Gatus::WorkerProvider::SetLog(LogFunctionType&& log) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_LogFunction = log ? std::move(log) : DefaultLogFunction;
}

void Gatus::WorkerProvider::Log(Verbosity verbosity, const std::string& message) {
    LogFunctionType log;
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        log = m_LogFunction;
    }
    // A user logger must not terminate the worker thread.
    try { log(verbosity, message); } catch (...) {}
}

void Gatus::WorkerProvider::WorkerLoop() {
    for (;;) {
        std::vector<std::string> tokens;
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Condition.wait(lock, [this] { return m_StopFlag || !m_PushTokens.empty(); });
            // Drop queued heartbeats on shutdown rather than report health after stopping.
            if (m_StopFlag) return;
            tokens.swap(m_PushTokens);
        }
        for (const auto& token : tokens) {
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                if (m_StopFlag) return;
            }
            try {
                m_Client->MakeRequest(HttpClientMethod::Post, m_ApiUrl, m_PushPath,
                    {{"Authorization", "Bearer " + token}}, {},
                    [this](std::int32_t status) {
                        if (status >= 200 && status < 300)
                            Log(Verbosity::Verbose, "Push done.");
                        else
                            Log(Verbosity::Error, "HTTP " + std::to_string(status) + " while push.");
                    });
            } catch (...) {
                Log(Verbosity::Error, "HTTP client failed while push.");
            }
        }
    }
}

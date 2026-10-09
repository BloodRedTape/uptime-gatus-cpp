#include "gatus/uptime.hpp"
#include "gatus/provider/worker_provider.hpp"
#include <stdexcept>
#include <utility>

Gatus::Uptime::Uptime(std::unique_ptr<Provider>&& provider, const std::string& token)
    : m_Provider(std::move(provider)), m_Token(token) {
    if (!m_Provider) throw std::invalid_argument("Gatus provider is required");
}

Gatus::Uptime::Uptime(std::unique_ptr<HttpClient>&& client, const std::string& url,
    const std::string& endpoint_key, const std::string& token)
    : Uptime(std::make_unique<WorkerProvider>(std::move(client), url, endpoint_key), token) {}

void Gatus::Uptime::Push() {
    m_Provider->Push(m_Token);
}

void Gatus::Uptime::SetLog(Provider::LogFunctionType&& log) {
    m_Provider->SetLog(std::move(log));
}

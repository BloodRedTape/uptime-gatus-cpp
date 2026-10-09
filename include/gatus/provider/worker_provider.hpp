#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "gatus/provider/provider.hpp"

namespace Gatus {

class WorkerProvider : public Provider {
    std::unique_ptr<HttpClient> m_Client;
    std::string m_ApiUrl;
    std::string m_PushPath;
    mutable std::mutex m_Mutex;
    std::condition_variable m_Condition;
    std::vector<std::string> m_PushTokens;
    bool m_StopFlag = false;
    LogFunctionType m_LogFunction = DefaultLogFunction;
    std::thread m_WorkerThread;
public:
    WorkerProvider(std::unique_ptr<HttpClient>&& client, const std::string& api_url,
        const std::string& endpoint_key);
    ~WorkerProvider() override;

    void Push(const std::string& token) override;
    bool AnyPending() const;
    void SetLog(LogFunctionType&& log) override;
private:
    void WorkerLoop();
    void Log(Verbosity verbosity, const std::string& message);
};

} // namespace Gatus

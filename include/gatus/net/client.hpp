#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace Gatus {

enum class HttpClientMethod { Post };
const char* ToString(HttpClientMethod method);

class HttpClient {
public:
    using CompleteCallbackType = std::function<void(std::int32_t)>;
    virtual ~HttpClient() = default;

    // Synchronous: invoke complete exactly once before returning; 0 means transport failure.
    virtual void MakeRequest(HttpClientMethod method, const std::string& hostname,
        const std::string& path, const std::map<std::string, std::string>& headers,
        std::string&& body, CompleteCallbackType&& complete) = 0;
};

} // namespace Gatus

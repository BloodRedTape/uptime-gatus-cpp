#pragma once

#include "gatus/net/client.hpp"

namespace Gatus {
class HttplibHttpClient : public HttpClient {
public:
    void MakeRequest(HttpClientMethod method, const std::string& hostname,
        const std::string& path, const std::map<std::string, std::string>& headers,
        std::string&& body, CompleteCallbackType&& complete) override;
};
} // namespace Gatus

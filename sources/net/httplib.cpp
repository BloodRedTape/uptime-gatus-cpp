#include "gatus/net/httplib.hpp"
#include <httplib.h>

void Gatus::HttplibHttpClient::MakeRequest(HttpClientMethod method,
    const std::string& hostname, const std::string& path,
    const std::map<std::string, std::string>& headers, std::string&& body,
    CompleteCallbackType&& complete) {
    std::int32_t status = 0;
    try {
        httplib::Client client(hostname);
        client.set_connection_timeout(10);
        client.set_read_timeout(10);
        client.set_write_timeout(10);
        if (method == HttpClientMethod::Post) {
            httplib::Headers request_headers(headers.begin(), headers.end());
            auto response = client.Post(path, request_headers, body,
                headers.count("Content-Type") ? headers.at("Content-Type") : "text/plain");
            if (response) status = response->status;
        }
    } catch (...) {}
    if (complete) complete(status);
}

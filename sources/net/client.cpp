#include "gatus/net/client.hpp"
#include <cassert>

const char* Gatus::ToString(HttpClientMethod method) {
    if (method == HttpClientMethod::Post) return "POST";
    assert(false);
    return "<None>";
}

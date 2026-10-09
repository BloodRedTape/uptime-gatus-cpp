#pragma once

#include "gatus/net/client.hpp"

namespace Gatus {

enum class Verbosity { Verbose, Display, Info, Warning, Error };
const char* ToString(Verbosity verbosity);

class Provider {
public:
    using LogFunctionType = std::function<void(Verbosity, const std::string&)>;
    static void DefaultLogFunction(Verbosity verbosity, const std::string& message);
    virtual ~Provider() = default;
    virtual void Push(const std::string& token) = 0;
    virtual void SetLog(LogFunctionType&& log) = 0;
};

} // namespace Gatus

#include "gatus/provider/provider.hpp"
#include <iostream>

const char* Gatus::ToString(Verbosity verbosity) {
    switch (verbosity) {
    case Verbosity::Verbose: return "Verbose";
    case Verbosity::Display: return "Display";
    case Verbosity::Info: return "Info";
    case Verbosity::Warning: return "Warning";
    case Verbosity::Error: return "Error";
    }
    return "Unknown";
}

void Gatus::Provider::DefaultLogFunction(Verbosity verbosity, const std::string& message) {
    std::cout << ToString(verbosity) << ":" << message << '\n';
}

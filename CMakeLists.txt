cmake_minimum_required(VERSION 3.14)
project(Uptime-Gatus-Cpp LANGUAGES CXX)

option(CMAKE_GATUS_USE_BOOST "Build Boost HTTP client" OFF)
option(CMAKE_GATUS_USE_HTTPLIB "Build cpp-httplib HTTP client" OFF)

find_package(Threads REQUIRED)
add_library(UptimeGatusCpp
    sources/uptime.cpp
    sources/provider/provider.cpp
    sources/provider/worker_provider.cpp
    sources/net/client.cpp
)
target_include_directories(UptimeGatusCpp PUBLIC include)
target_compile_features(UptimeGatusCpp PUBLIC cxx_std_17)
target_link_libraries(UptimeGatusCpp PUBLIC Threads::Threads)

if(CMAKE_GATUS_USE_BOOST)
    find_package(Boost REQUIRED)
    find_package(OpenSSL REQUIRED)
    target_sources(UptimeGatusCpp PRIVATE sources/net/boost.cpp)
    target_link_libraries(UptimeGatusCpp PUBLIC Boost::boost OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(UptimeGatusCpp PUBLIC GATUS_USE_BOOST=1)
endif()

if(CMAKE_GATUS_USE_HTTPLIB)
    if(NOT TARGET httplib AND NOT TARGET httplib::httplib)
        find_package(httplib CONFIG REQUIRED)
    endif()
    find_package(OpenSSL REQUIRED)
    target_sources(UptimeGatusCpp PRIVATE sources/net/httplib.cpp)
    if(TARGET httplib::httplib)
        target_link_libraries(UptimeGatusCpp PUBLIC httplib::httplib)
    else()
        target_link_libraries(UptimeGatusCpp PUBLIC httplib)
    endif()
    target_link_libraries(UptimeGatusCpp PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(UptimeGatusCpp PUBLIC GATUS_USE_HTTPLIB=1 CPPHTTPLIB_OPENSSL_SUPPORT=1)
endif()

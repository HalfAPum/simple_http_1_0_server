cmake_minimum_required(VERSION 3.28)
project(simple_http_1_0_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_http_1_0_server main.cpp
        request/line/RequestLine.h
        request/line/RequestLine.cpp
        response/StatusCode.h
        response/ResponseStrategy.h
        response/SimpleResponseStrategy.h
        response/SimpleResponseStrategy.cpp
        response/FullResponseStrategy.h
        response/FullResponseStrategy.cpp
        request/line/RequestLineValidator.h
        request/line/RequestLineValidator.cpp
        request/headers/RequestHeadersValidator.h
        request/headers/RequestHeadersValidator.cpp
        request/headers/RequestHeaders.h)
if(WIN32)
    target_link_libraries(simple_http_1_0_server wsock32 ws2_32)
endif()

cmake_minimum_required(VERSION 3.28)
project(simple_http_1_0_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_http_1_0_server main.cpp
        RequestLine.h
        RequestLine.cpp
        StatusCode.h)
if(WIN32)
    target_link_libraries(simple_http_1_0_server wsock32 ws2_32)
endif()

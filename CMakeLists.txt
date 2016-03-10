cmake_minimum_required(VERSION 3.3)
project(ipk_proj1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc url_parser.cc socket_handler.cc)
add_executable(ipk_proj1 ${SOURCE_FILES})
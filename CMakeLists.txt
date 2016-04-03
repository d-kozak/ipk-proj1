cmake_minimum_required(VERSION 3.3)
project(ipk_proj1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/david/Documents/studium/4.semestr/IPK/ipk-proj1/bin")

set(SOURCE_FILES webclient.cc url_parser.cc socket_handler.cc RedirHandler.cc)

add_executable(ipk_proj1 ${SOURCE_FILES})
cmake_minimum_required(VERSION 3.15)
project(bluetooth-rfcomm-server)

set(CMAKE_CXX_STANDARD 17)

add_executable(bluetooth-rfcomm-server main.cpp FdBuffer.cpp FdBuffer.h SocketServer.cpp SocketServer.h)

target_link_libraries(bluetooth-rfcomm-server bluetooth)
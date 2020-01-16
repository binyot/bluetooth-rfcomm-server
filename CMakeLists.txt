cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
project(btserver CXX C)
add_library(btserver SHARED
        src/FdBuffer.cpp
        src/SocketServer.cpp)
target_link_libraries(btserver bluetooth)
set_target_properties(btserver PROPERTIES PUBLIC_HEADER include/SocketServer.h)
target_include_directories(btserver PRIVATE include)
target_include_directories(btserver PRIVATE src)
include(GNUInstallDirs)
install(TARGETS btserver
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
add_executable(test src/example.cpp)
target_link_libraries(test btserver)
target_include_directories(test PRIVATE include)
target_include_directories(test PRIVATE src)
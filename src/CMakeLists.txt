
set(CMAKE_CXX_STANDARD 17)

add_library(btserver STATIC SocketServer.cpp FdBuffer.cpp)
target_include_directories(btserver PRIVATE ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(btserver PROPERTIES PUBLIC_HEADER "SocketServer.h;FdBuffer.h")

add_executable(test main.cpp)
target_link_libraries(test btserver bluetooth)

install(TARGETS btserver
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib/
        PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/)
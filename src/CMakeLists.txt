add_executable(server TCP/tcpserv.cpp)
add_executable(client TCP/tcpclient.cpp)

add_library(TCP STATIC TCP/TCP.hpp TCP/Server.cpp)

include(CompileOptions)

set_compile_options(server)
set_compile_options(client)
set_compile_options(TCP)

target_include_directories(TCP PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(TCP PRIVATE m)
target_link_libraries(server PRIVATE TCP)

target_link_libraries(server)
target_link_libraries(client)
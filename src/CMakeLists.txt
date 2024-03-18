add_executable(server TCP/tcpserv.cpp)
add_executable(client TCP/tcpclient.cpp)

include(CompileOptions)

set_compile_options(server)
set_compile_options(client)

target_link_libraries(server)
target_link_libraries(client)
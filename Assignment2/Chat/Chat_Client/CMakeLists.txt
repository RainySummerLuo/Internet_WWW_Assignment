cmake_minimum_required(VERSION 3.14)

project(Chat_Client)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS -static)

link_libraries(ws2_32)

add_executable(Chat_Client client.cpp)

# add_executable(Chat_Client des.cpp)
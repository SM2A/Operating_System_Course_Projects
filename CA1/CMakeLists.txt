cmake_minimum_required(VERSION 3.17)
project(OS C)

set(CMAKE_C_STANDARD 99)

add_executable(OS_CA1_CLIENT Source/client.c Source/structure.h)
add_executable(OS_CA1_SERVER Source/server.c Source/structure.h)
cmake_minimum_required(VERSION 3.12)
project(pc C)

set(CMAKE_C_STANDARD 99)

find_package (Threads)

add_executable(sequential sequential.c)
add_executable(parallel parallel.c)

target_link_libraries(sequential m)
target_link_libraries(parallel m)
target_link_libraries (parallel ${CMAKE_THREAD_LIBS_INIT})
cmake_minimum_required(VERSION 4.1)
project(replicate)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-g -O0 -Wall -Wextra")
include_directories(include/)

file(GLOB SRC_FILES src/*.c)

add_executable(replicate ${SRC_FILES})
target_include_directories(replicate PRIVATE include)
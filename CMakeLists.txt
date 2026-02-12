cmake_minimum_required(VERSION 4.1)
project(replicate)

# Make clangd see headers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Other C/C++ setup
set(CMAKE_C_STANDARD 23)

# Set log level for all targets
add_compile_definitions(LOG_LEVEL=LOG_LEVEL_INFO)

# Include headers
include_directories(include/)

# Gather all source files
file(GLOB SRC_FILES src/*.c)

# Create the executable
add_executable(replicate ${SRC_FILES})
target_include_directories(replicate PRIVATE include)

# Copy textfile to build
configure_file(oi.txt oi.txt COPYONLY)

# Platform-specific settings
if (WIN32)
    message(STATUS "Configuring for Windows")

    # Compiler flags for MSVC
    if (MSVC)
        target_compile_options(replicate PRIVATE /W4 /EHsc /Zi /D_CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(replicate PRIVATE -g -O0 -Wall -Wextra)
    endif()
elseif(UNIX)
    message(STATUS "Configuring for Linux/UNIX")
    target_compile_options(
        replicate PRIVATE -g -O0 -Wall -Wextra -Wpedantic -Werror -Wuninitialized -Wmaybe-uninitialized
        -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wstrict-aliasing=2 -Wpointer-arith
        -Warray-bounds -Wnull-dereference -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
        -Wredundant-decls -Wshadow -Wformat=2 -Wformat-security -Wwrite-strings -Wvla -Wno-padded 
        -Wno-declaration-after-statement
    )
endif()
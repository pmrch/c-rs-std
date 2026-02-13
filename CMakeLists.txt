cmake_minimum_required(VERSION 4.1)
project(replicate)

# Make clangd see headers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Other C/C++ setup
set(CMAKE_C_STANDARD 23)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set log level based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(LOG_LEVEL=LOG_LEVEL_DEBUG)
    message(STATUS "Log level: DEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(LOG_LEVEL=LOG_LEVEL_INFO)
    message(STATUS "Log level: INFO")
endif()

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
if(WIN32)
    message(STATUS "Configuring for Windows")

    if(MSVC)
        message(STATUS "Using MSVC compiler")
        
        # Debug flags
        target_compile_options(replicate PRIVATE
            /W4 /WX /EHsc /permissive- /sdl
            /D_CRT_SECURE_NO_WARNINGS
            /w14242 /w14826 /w14287 /w14296 /w14311
            /w14545 /w14546 /w14547 /w14549 /w14555
            /w14905 /w14906
            
            # Debug vs Release
            $<$<CONFIG:Debug>:/Zi /Od>
            $<$<CONFIG:Release>:/O2 /Ob2 /GL>
        )
        
        # Release-specific linker flags
        target_link_options(replicate PRIVATE
            $<$<CONFIG:Release>:/LTCG>  # Link-time code generation
        )
    else()
        # MinGW or other Windows compiler
        target_compile_options(replicate PRIVATE
            -Wall -Wextra
            $<$<CONFIG:Debug>:-g -O0>
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
        )
    endif()
    
elseif(UNIX)
    message(STATUS "Configuring for Linux/UNIX")
    
    target_compile_options(replicate PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wuninitialized -Wmaybe-uninitialized
        -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual
        -Wstrict-aliasing=2 -Wpointer-arith -Warray-bounds
        -Wnull-dereference -Wmissing-prototypes -Wstrict-prototypes
        -Wold-style-definition -Wredundant-decls -Wshadow
        -Wformat=2 -Wformat-security -Wwrite-strings -Wvla
        -Wno-padded -Wno-declaration-after-statement
        
        # Debug vs Release
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
    )
endif()
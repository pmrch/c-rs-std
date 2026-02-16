cmake_minimum_required(VERSION 3.12) # Use 3.12+ for CONFIGURE_DEPENDS
project(replicate)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)


add_compile_definitions(LOG_LEVEL=LOG_LEVEL_INFO)  # Only show info+ higher

# --- Build Type & Logging ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(LOG_LEVEL=LOG_LEVEL_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(LOG_LEVEL=LOG_LEVEL_INFO)
endif()

# --- Source Management ---
file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.c")

# Separate main.c from the rest of the logic
set(CORE_SOURCES ${SRC_FILES})
list(FILTER CORE_SOURCES EXCLUDE REGEX "src/main.c")

# 1. Create a library of your core logic (used by both app and tests)
add_library(core_logic STATIC ${CORE_SOURCES})
target_include_directories(core_logic PUBLIC include)

# 2. Update your main executable to use the library
# Ensure main.c is present; if it's not in SRC_FILES, explicitly add "src/main.c"
add_executable(replicate src/main.c)
target_link_libraries(replicate PRIVATE core_logic)
configure_file(oi.txt oi.txt COPYONLY)

# --- Compiler Settings (Inherited by replicate and tests) ---
# We use a separate target for compiler settings to avoid duplication
add_library(project_warnings INTERFACE)

if(WIN32)
    if(MSVC)
        target_compile_options(project_warnings INTERFACE 
            /W4 /WX /EHsc /permissive- /sdl /D_CRT_SECURE_NO_WARNINGS
            $<$<CONFIG:Debug>:/Zi /Od>
            $<$<CONFIG:Release>:/O2 /Ob2 /GL>
        )
        target_link_options(project_warnings INTERFACE $<$<CONFIG:Release>:/LTCG>)
    else()
        target_compile_options(project_warnings INTERFACE -Wall -Wextra $<$<CONFIG:Debug>:-g -O0> $<$<CONFIG:Release>:-O3 -DNDEBUG>)
    endif()
elseif(UNIX)
    target_compile_options(project_warnings INTERFACE 
        -Wall -Wextra -Wpedantic -Werror -Wuninitialized -Wmaybe-uninitialized
        -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual
        -Wstrict-aliasing=2 -Wpointer-arith -Warray-bounds -Wnull-dereference
        -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
        -Wredundant-decls -Wshadow -Wformat=2 -Wformat-security -Wwrite-strings -Wvla
        -Wno-padded -Wno-declaration-after-statement
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
    )
endif()

# Apply warnings to our main targets
target_link_libraries(replicate PRIVATE project_warnings)
target_link_libraries(core_logic PRIVATE project_warnings)

# --- AUTOMATED TEST DISCOVERY ---
enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "tests/*.c")

foreach(test_path ${TEST_SOURCES})
    get_filename_component(test_name ${test_path} NAME_WE)
    
    # Each test is its own executable
    add_executable(${test_name} ${test_path})
    
    # Link to the logic library and the shared warnings/flags
    target_link_libraries(${test_name} PRIVATE core_logic project_warnings)
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
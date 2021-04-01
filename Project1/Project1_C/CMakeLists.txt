# CMakeLists.txt for project 1 of CS{4/6}290 and ECE{4/6}100 Spring 2020

cmake_minimum_required (VERSION 3.1) # Earlier versions won't support this file

# C project
project(cachesim
        DESCRIPTION "Cache hit-miss simulator"
        LANGUAGES C)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(SRC_FILES "${CMAKE_SOURCE_DIR}/src/cachesim_driver.c"
              "${CMAKE_SOURCE_DIR}/src/cache.c"
              "${CMAKE_SOURCE_DIR}/src/cache.h"
              "${CMAKE_SOURCE_DIR}/src/util/jsmn.h")

# Sets required flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set flags for stricter warnings and errors
if( "${CMAKE_C_COMPILER_ID}" MATCHES GNU )
    message(STATUS "Using gcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -Wpointer-arith -Wsign-conversion -Wconversion -pedantic-errors")
elseif( "${CMAKE_C_COMPILER_ID}" MATCHES Clang )
    message(STATUS "Using clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -Wpointer-arith -Wsign-conversion -Wconversion -pedantic-errors")
endif()

# Enable debugging using gdb or lldb depending on operating system
set(CMAKE_BUILD_TYPE Debug)

# Generate executable
add_executable(cachesim ${SRC_FILES})

cmake_minimum_required(VERSION 4.1.1)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(parser ./src/parser.cpp ./src/parser.h)
add_library(wav ./src/wav.cpp ./src/wav.h)
add_library(processor ./src/soundp.cpp ./src/soundp.h)
target_link_libraries(processor wav)

add_executable(soundp ./src/main.cpp)
target_link_libraries(soundp parser wav processor)

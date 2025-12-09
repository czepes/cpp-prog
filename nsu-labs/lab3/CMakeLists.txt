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

add_library(processor
  ./src/wav.cpp ./src/wav.h
  ./src/parser.cpp ./src/parser.h
  ./src/config-parser.cpp ./src/config-parser.h
  ./src/converter.cpp ./src/converter.h
  ./src/converters/muter.cpp ./src/converters/muter.h
  ./src/converters/mixer.cpp ./src/converters/mixer.h
  ./src/converters/cropper.cpp ./src/converters/cropper.h
  ./src/converters/gainer.cpp ./src/converters/gainer.h
  ./src/soundp.cpp ./src/soundp.h
)

add_executable(soundp ./src/main.cpp)
target_link_libraries(soundp processor)

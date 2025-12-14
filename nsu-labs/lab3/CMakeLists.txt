cmake_minimum_required(VERSION 4.1.1)
project(lab3)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

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
  ./src/wav.cpp
  ./src/parser.cpp
  ./src/config-parser.cpp
  ./src/converter.cpp
  ./src/converters/muter.cpp
  ./src/converters/mixer.cpp
  ./src/converters/cropper.cpp
  ./src/converters/gainer.cpp
  ./src/soundp.cpp
)

add_executable(soundp ./src/main.cpp)
target_link_libraries(soundp processor)

add_library(processor-coverage
  ./src/wav.cpp
  ./src/config-parser.cpp
  ./src/converter.cpp
  ./src/converters/muter.cpp
  ./src/converters/mixer.cpp
  ./src/converters/cropper.cpp
  ./src/converters/gainer.cpp
  ./src/soundp.cpp
)
target_compile_options(processor-coverage PRIVATE --coverage -fprofile-arcs -ftest-coverage)

add_library(no-coverage ./src/parser.cpp)
target_link_libraries(no-coverage processor-coverage)

add_executable(soundtest ./test/test.cpp)
target_link_libraries(soundtest GTest::gtest_main processor-coverage no-coverage gcov)
target_link_options(soundtest PRIVATE --coverage)

add_custom_target(
  gcovr
  COMMAND make soundtest
  COMMAND ${CMAKE_BINARY_DIR}/soundtest
  COMMAND gcovr -r ${PROJECT_SOURCE_DIR} --html-details gcovr.html --txt --merge-mode-functions=separate
  COMMAND echo
          "Detailed coverage report generated in ${CMAKE_BINARY_DIR}/gcovr.html"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS soundtest
)

gtest_discover_tests(soundtest)

cmake_minimum_required(VERSION 4.1.1)
project(lab4)

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

add_library(csv_headers INTERFACE)
target_sources(csv_headers INTERFACE
    ./src/csv-parser.hpp
    ./src/tuple-print.hpp
)
target_compile_options(csv_headers INTERFACE --coverage)

add_executable(parse-csv ./src/main.cpp)

add_executable(test-csv ./test/test.cpp)
target_link_libraries(test-csv PRIVATE csv_headers GTest::gtest_main)
target_link_options(test-csv PRIVATE --coverage)

add_custom_target(
  gcovr
  COMMAND make test-csv
  COMMAND ${CMAKE_BINARY_DIR}/test-csv
  COMMAND gcovr -r ${PROJECT_SOURCE_DIR} --html-details gcovr.html --txt --merge-mode-functions=separate
          --exclude-unreachable-branches
          --exclude-throw-branches
          --filter=".*/src/.*"
  COMMAND echo
          "Detailed coverage report generated in ${CMAKE_BINARY_DIR}/gcovr.html"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS test-csv
)

gtest_discover_tests(test-csv)

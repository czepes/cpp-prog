cmake_minimum_required(VERSION 4.1.1)
project(lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

add_library(bitarray ./src/bit-array.cpp)
target_compile_options(bitarray PRIVATE -g -O0 --coverage -fprofile-arcs
                                        -ftest-coverage)

add_executable(bitarraytest ./test/test.cpp)
target_link_libraries(bitarraytest GTest::gtest_main bitarray gcov)
target_link_options(bitarraytest PRIVATE --coverage)

add_custom_target(
  coverage
  COMMAND make bitarraytest
  COMMAND ${CMAKE_BINARY_DIR}/bitarraytest
  COMMAND lcov --capture --directory . --output-file coverage.info
  COMMAND lcov --remove coverage.info '/usr/include/*' --output-file
          coverage.info
  COMMAND genhtml coverage.info --output-directory coverage-report
  COMMAND echo "=== Coverage Report ==="
  COMMAND
    echo
    "Coverage report generated in ${CMAKE_BINARY_DIR}/coverage-report/index.html"
  COMMAND echo "=== Coverage summary ==="
  COMMAND lcov --summary coverage.info
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS bitarraytest)

gtest_discover_tests(bitarraytest)

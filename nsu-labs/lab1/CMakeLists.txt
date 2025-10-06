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

add_library(bitarray_lib ./src/bit-array.cpp)
target_compile_options(bitarray_lib PRIVATE -g -O0 --coverage -fprofile-arcs
                                            -ftest-coverage)

add_executable(bitarray_test ./test/test.cpp)
target_link_libraries(bitarray_test GTest::gtest_main bitarray_lib gcov)
target_link_options(bitarray_test PRIVATE --coverage)

add_custom_target(
  coverage
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
  DEPENDS bitarray_test)

gtest_discover_tests(bitarray_test)

cmake_minimum_required(VERSION 4.1.1)
project(lab2)

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

add_library(parser ./src/parser.cpp ./src/parser.h)
add_library(renderer ./src/renderer.cpp ./src/renderer.h)
add_library(controller ./src/controller.cpp ./src/controller.h)
add_library(simulator ./src/simulator.cpp ./src/simulator.h)
add_library(patterns ./src/patterns.cpp ./src/patterns.h)

add_library(gsimulator ./src/simulator.cpp ./src/simulator.h)
target_compile_options(gsimulator PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)

add_executable(game ./src/game.cpp)
target_link_libraries(game parser renderer simulator patterns controller)

add_executable(gametest ./test/test.cpp)
target_link_libraries(gametest GTest::gtest_main parser renderer gsimulator patterns controller gcov)
target_link_options(gametest PRIVATE --coverage)

add_custom_target(
  gcovr
  COMMAND make gametest
  COMMAND ${CMAKE_BINARY_DIR}/gametest
  COMMAND gcovr -r ${PROJECT_SOURCE_DIR} --html-details gcovr.html --txt
  COMMAND echo
          "Detailed coverage report generated in ${CMAKE_BINARY_DIR}/gcovr.html"
)

add_custom_target(
  coverage
  COMMAND make gametest
  COMMAND ${CMAKE_BINARY_DIR}/gametest
  COMMAND lcov --capture --directory . --output-file coverage.info
  COMMAND lcov --remove coverage.info '/usr/include/*' --output-file
          coverage.info
  COMMAND lcov --list coverage.info
  COMMAND genhtml coverage.info --output-directory coverage-report
  COMMAND echo "=== Coverage Report ==="
  COMMAND
    echo
    "Coverage report generated in ${CMAKE_BINARY_DIR}/coverage-report/index.html"
  COMMAND echo "=== Coverage summary ==="
  COMMAND lcov --summary coverage.info
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS gametest)

gtest_discover_tests(gametest)

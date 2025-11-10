cmake_minimum_required(VERSION 4.1.1)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# FetchContent_MakeAvailable(googletest)

# include(GoogleTest)

# enable_testing()

add_library(renderer ./src/renderer.cpp ./src/renderer.h)
# target_compile_options(devtest PRIVATE -g -O0 --coverage -fprofile-arcs
#                                         -ftest-coverage)
add_library(controller ./src/controller.cpp ./src/controller.h)
add_library(simulator ./src/simulator.cpp ./src/simulator.h)
add_library(parser ./src/parser.cpp ./src/parser.h)

add_executable(rensim ./src/rensim.cpp)
add_executable(par ./src/par.cpp)
add_executable(game ./src/game.cpp)

target_link_libraries(rensim renderer simulator)
target_link_libraries(par parser)
target_link_libraries(game parser renderer simulator controller)

# target_link_libraries(bitarraytest GTest::gtest_main bitarray gcov)
# target_link_options(bitarraytest PRIVATE --coverage)

# add_custom_target(
#   gcovr
#   COMMAND make bitarraytest
#   COMMAND ${CMAKE_BINARY_DIR}/bitarraytest
#   COMMAND gcovr -r ${PROJECT_SOURCE_DIR} --html-details gcovr.html --txt
#   COMMAND echo
#           "Detailed coverage report generated in ${CMAKE_BINARY_DIR}/gcovr.html"
# )
# add_custom_target(
#   coverage
#   COMMAND make bitarraytest
#   COMMAND ${CMAKE_BINARY_DIR}/bitarraytest
#   COMMAND lcov --capture --directory . --output-file coverage.info
#   COMMAND lcov --remove coverage.info '/usr/include/*' --output-file
#           coverage.info
#   COMMAND lcov --list coverage.info
#   COMMAND genhtml coverage.info --output-directory coverage-report
#   COMMAND echo "=== Coverage Report ==="
#   COMMAND
#     echo
#     "Coverage report generated in ${CMAKE_BINARY_DIR}/coverage-report/index.html"
#   COMMAND echo "=== Coverage summary ==="
#   COMMAND lcov --summary coverage.info
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#   DEPENDS bitarraytest)
#
# gtest_discover_tests(bitarraytest)

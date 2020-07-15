cmake_minimum_required(VERSION 3.12)
project(cpp_channel)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage")

set(PROJECT_VERSION 0.2.0)

add_library(shared SHARED channel.hpp)
target_link_libraries(shared pthread)
set_target_properties(shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shared PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(shared PROPERTIES LIBRARY_OUTPUT_NAME channel)
install(TARGETS shared
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Testing framework
include(FetchContent)
if (NOT googletest_POPULATED)
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/release-1.10.0.zip)
    FetchContent_Populate(googletest)
    include_directories(${googletest_SOURCE_DIR}/googletest/include/)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif ()

# Test macro
macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gtest_main pthread)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro()

# Test
package_add_test(channel_test11 channel_test.cpp)
package_add_test(channel_test14 channel_test.cpp)
package_add_test(channel_test17 channel_test.cpp)
package_add_test(channel_iterator_test channel_iterator_test.cpp)

set_target_properties(channel_test14 PROPERTIES CXX_STANDARD 14 CMAKE_CXX_STANDARD_REQUIRED YES)
set_target_properties(channel_test17 PROPERTIES CXX_STANDARD 17 CMAKE_CXX_STANDARD_REQUIRED YES)

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/v1.5.0.zip)
    FetchContent_Populate(benchmark)
    add_subdirectory(${benchmark_SOURCE_DIR})
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)

add_subdirectory(examples)

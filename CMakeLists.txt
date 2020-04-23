cmake_minimum_required(VERSION 3.12)
project(cpp_channel)

set(CMAKE_CXX_STANDARD 11)
set(PROJECT_VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_library(shared SHARED channel.h)
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
    include_directories(${googletest_SOURCE_DIR}/googlemock/include/)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif ()

# Test macro
macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gtest_main pthread)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro()

# Test
package_add_test(channel_test channel_test.cpp)

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/v1.5.0.zip)
    FetchContent_Populate(benchmark)
    add_subdirectory(${benchmark_SOURCE_DIR})
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)

# Example app
add_executable(channel_example example.cpp)
target_link_libraries(channel_example pthread)

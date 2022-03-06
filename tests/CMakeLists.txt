# Testing framework
include(FetchContent)
if (NOT googletest_POPULATED)
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/release-1.11.0.zip)
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

# Tests
package_add_test(channel_test channel_test.cpp blocking_iterator_test.cpp)

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/v1.6.1.zip)
    FetchContent_Populate(benchmark)
    add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)

# Testing framework
include(FetchContent)
if (NOT googletest_POPULATED)
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/release-1.11.0.zip)
    FetchContent_Populate(googletest)
    include_directories(${googletest_SOURCE_DIR}/googletest/include/)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif ()

# Test macro
function(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})

    set_target_warnings(${TESTNAME} PRIVATE)
    target_link_libraries(${TESTNAME} gtest gtest_main pthread -lgcov -lubsan)
    target_compile_options(${TESTNAME} PRIVATE --coverage -fsanitize=undefined)

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endfunction()

# Tests
package_add_test(channel_test11 channel_test.cpp blocking_iterator_test.cpp)
package_add_test(channel_test14 channel_test.cpp blocking_iterator_test.cpp)
package_add_test(channel_test17 channel_test.cpp blocking_iterator_test.cpp)

set_target_properties(channel_test14 PROPERTIES CXX_STANDARD 14 CMAKE_CXX_STANDARD_REQUIRED YES)
set_target_properties(channel_test17 PROPERTIES CXX_STANDARD 17 CMAKE_CXX_STANDARD_REQUIRED YES)

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/v1.6.1.zip)
    FetchContent_Populate(benchmark)
    add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)
set_target_warnings(channel_benchmark PRIVATE INTERFACE)

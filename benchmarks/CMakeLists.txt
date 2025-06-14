include(FetchContent)
if(NOT benchmark_POPULATED)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip
                                       DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(benchmark)
endif()

function(package_add_benchmark TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    set_target_warnings(${TESTNAME} PRIVATE)
    target_link_libraries(${TESTNAME} benchmark)
endfunction()

package_add_benchmark(channel_benchmark channel_benchmark.cpp)

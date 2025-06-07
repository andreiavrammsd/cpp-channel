if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

# Testing framework
if (MSVC)
    option(
      gtest_force_shared_crt
      "Use shared (DLL) run-time lib even when Google Test is built as static lib."
      ON)
endif()

include(FetchContent)
if (NOT googletest_POPULATED)
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
    FetchContent_MakeAvailable(googletest)
endif ()

# Test macro
function(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})

    set_target_warnings(${TESTNAME} PRIVATE)
    target_link_libraries(${TESTNAME} gtest gtest_main)

    if (CPP_CHANNEL_COVERAGE)
        target_link_libraries(${TESTNAME} -lgcov)
        target_compile_options(${TESTNAME} PRIVATE --coverage)
    endif ()

    if (CPP_CHANNEL_SANITIZERS)
        target_link_libraries(${TESTNAME} -lubsan)
        target_compile_options(${TESTNAME} PRIVATE -fsanitize=undefined)
    endif ()

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})

    add_dependencies(tests ${TESTNAME})
endfunction()

add_custom_target(tests)

# Tests
package_add_test(channel_test channel_test.cpp blocking_iterator_test.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # Disable warnings about C++17 extensions
    target_compile_options(channel_test PRIVATE -Wno-c++17-extensions)
endif()

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/refs/tags/v1.8.5.zip)
    FetchContent_MakeAvailable(benchmark)
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)
set_target_warnings(channel_benchmark PRIVATE)

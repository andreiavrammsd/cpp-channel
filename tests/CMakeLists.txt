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
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip)
    FetchContent_MakeAvailable(googletest)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST_TESTS OFF CACHE BOOL "" FORCE)
endif ()

# Test macro
function(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})

    target_compile_options(${TESTNAME} PRIVATE -std=c++${CMAKE_CXX_STANDARD})

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

    if (CPP_CHANNEL_SANITIZE_THREADS)
        target_link_libraries(${TESTNAME} -fsanitize=thread) 
        target_compile_options(${TESTNAME} PRIVATE -fsanitize=thread)
    endif ()

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_tests_properties(${TESTNAME} PROPERTIES LABELS "channel_tests")

    add_dependencies(channel_tests ${TESTNAME})
endfunction()

add_custom_target(channel_tests)

# Tests
package_add_test(channel_test channel_test.cpp)
package_add_test(blocking_iterator_test blocking_iterator_test.cpp)
package_add_test(storage_test storage_test.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # Disable warnings about C++17 extensions
    target_compile_options(channel_test PRIVATE -Wno-c++17-extensions)
endif()

# Benchmark
if (NOT benchmark_POPULATED)
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip)
    FetchContent_MakeAvailable(benchmark)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
endif ()

add_executable(channel_benchmark channel_benchmark.cpp)
target_link_libraries(channel_benchmark benchmark)
set_target_warnings(channel_benchmark PRIVATE)
target_compile_options(channel_benchmark PRIVATE -std=c++${CMAKE_CXX_STANDARD})

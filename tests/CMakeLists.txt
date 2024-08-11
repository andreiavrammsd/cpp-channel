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

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${TESTNAME} -lgcov -lubsan)
        target_compile_options(${TESTNAME} PRIVATE --coverage -fsanitize=undefined)
    endif ()

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})

    add_dependencies(tests ${TESTNAME})
endfunction()

add_custom_target(tests)

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
set_target_warnings(channel_benchmark PRIVATE)

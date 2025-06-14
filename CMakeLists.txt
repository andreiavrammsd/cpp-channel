cmake_minimum_required(VERSION 3.15)
project(cpp_channel VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(warnings)

add_library(msd_channel INTERFACE)
target_include_directories(msd_channel INTERFACE include)

option(CPP_CHANNEL_BUILD_TESTS "Build all of cpp_channel's own tests." OFF)
option(CPP_CHANNEL_BUILD_BENCHMARKS "Build all of cpp_channel's own benchmark tests." OFF)
option(CPP_CHANNEL_BUILD_EXAMPLES "Build cpp_channel's example programs." OFF)
option(CPP_CHANNEL_COVERAGE "Generate test coverage." OFF)
option(CPP_CHANNEL_SANITIZERS "Build with sanitizers." OFF)
option(CPP_CHANNEL_SANITIZE_THREADS "Build with thread sanitizer." OFF)

if(CPP_CHANNEL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CPP_CHANNEL_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(CPP_CHANNEL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(DIRECTORY include/ DESTINATION include)

cmake_minimum_required(VERSION 3.12)
project(cpp_channel)
set(PROJECT_VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(warnings)

include_directories(include)

add_library(msd_channel INTERFACE)
target_include_directories(msd_channel INTERFACE include)

option(CPP_CHANNEL_BUILD_TESTS "Build all of cpp_channel's own tests." OFF)
option(CPP_CHANNEL_BUILD_EXAMPLES "Build cpp_channel's example programs." OFF)
option(CPP_CHANNEL_COVERAGE "Generate test coverage." OFF)
option(CPP_CHANNEL_SANITIZERS "Build with sanitizers." OFF)
option(CPP_CHANNEL_SANITIZE_THREADS "Build with thread sanitizer." OFF)

if (CPP_CHANNEL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (CPP_CHANNEL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(DIRECTORY include/ DESTINATION include)

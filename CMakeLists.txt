cmake_minimum_required(VERSION 3.12)
project(cpp_channel)
set(PROJECT_VERSION 0.8.3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CXX_EXTENSIONS NO)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(warnings)

include_directories(include)

add_library(msd_channel INTERFACE)
target_include_directories(msd_channel INTERFACE include)

option(cpp_channel_build_tests "Build all of cpp_channel's own tests." OFF)
option(cpp_channel_build_examples "Build cpp_channel's example programs." OFF)

if (cpp_channel_build_tests)
    enable_testing()
    add_subdirectory(tests)
endif()

if (cpp_channel_build_examples)
    add_subdirectory(examples)
endif()

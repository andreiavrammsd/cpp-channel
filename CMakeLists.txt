cmake_minimum_required(VERSION 3.12)
project(cpp_channel)
set(PROJECT_VERSION 0.7.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(warnings)

include_directories(include)

add_library(msd_channel INTERFACE)
target_include_directories(msd_channel INTERFACE include)

enable_testing()
add_subdirectory(tests)

add_subdirectory(examples)

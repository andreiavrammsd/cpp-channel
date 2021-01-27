cmake_minimum_required(VERSION 3.12)
project(cpp_channel)
set(PROJECT_VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage -fsanitize=undefined")

include_directories(include)

enable_testing()
add_subdirectory(tests)

add_subdirectory(examples)

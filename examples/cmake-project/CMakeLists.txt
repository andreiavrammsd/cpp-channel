cmake_minimum_required(VERSION 3.16)
project(cmake_project)
set(PROJECT_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage")

add_executable(cmake_project src/main.cpp)
target_link_libraries(cmake_project pthread)
# OR
# target_link_libraries(cmake_project pthread msd_channel)

include(FetchContent)
if (NOT channel_POPULATED)
    FetchContent_Declare(channel URL https://github.com/andreiavrammsd/cpp-channel/archive/v0.7.0.zip)
    FetchContent_Populate(channel)
    include_directories(${channel_SOURCE_DIR}/include)
    # OR
    # add_subdirectory(${channel_SOURCE_DIR}/)
endif ()

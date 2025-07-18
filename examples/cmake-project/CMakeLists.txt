cmake_minimum_required(VERSION 3.15)
project(cmake_project)
set(PROJECT_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage")

add_executable(cmake_project src/main.cpp)

include(FetchContent)

if(NOT channel_POPULATED)
    FetchContent_Declare(channel URL https://github.com/andreiavrammsd/cpp-channel/archive/v1.3.0.zip
                                     DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_Populate(channel)
    include_directories(${channel_SOURCE_DIR}/include)
    # OR add_subdirectory(${channel_SOURCE_DIR}/) target_link_libraries(cmake_project msd_channel)
endif()

cmake_minimum_required(VERSION 3.12)
project(cpp_channel)
set(PROJECT_VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage")

add_library(shared SHARED include/channel.hpp)
set_target_properties(shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shared PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(shared PROPERTIES LIBRARY_OUTPUT_NAME channel)
install(TARGETS shared
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include_directories(include)
add_subdirectory(tests)
add_subdirectory(examples)

add_executable(example_basic basic.cpp)

add_executable(example_move move.cpp)

add_executable(example_multithreading multithreading.cpp)
target_link_libraries(example_multithreading pthread)

function(add_example NAME)
    add_executable(${NAME} ${ARGN})

    set_target_warnings(${NAME} PRIVATE)
    target_link_libraries(${NAME} -ltsan)
    target_compile_options(${NAME} PRIVATE -fsanitize=thread)
endfunction()

add_example(example_basic basic.cpp)
add_example(example_close close.cpp)
add_example(example_move move.cpp)
add_example(example_multithreading multithreading.cpp)
add_example(example_streaming streaming.cpp)

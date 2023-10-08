function(add_example NAME)
    add_executable(${NAME} ${ARGN})

    set_target_warnings(${NAME} PRIVATE)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${NAME} -ltsan)
        target_compile_options(${NAME} PRIVATE -fsanitize=thread)
    endif()    

    add_dependencies(examples ${NAME})
endfunction()

add_custom_target(examples)

# Examples
add_example(example_basic basic.cpp)
add_example(example_close close.cpp)
add_example(example_move move.cpp)
add_example(example_multithreading multithreading.cpp)
add_example(example_streaming streaming.cpp)

function(add_example NAME)
    add_executable(${NAME} ${ARGN})

    set_target_warnings(${NAME} PRIVATE)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${NAME} PRIVATE -fsanitize=thread)
        target_link_libraries(${NAME} -ltsan)
    endif()

    add_dependencies(examples ${NAME})
endfunction()

function(run_example NAME)
    add_custom_command(
        TARGET ${NAME}
        POST_BUILD
        COMMAND ${NAME}
        COMMENT "Running example: ${NAME}")
endfunction()

add_custom_target(examples)

# Examples
add_example(example_basic basic.cpp)
run_example(example_basic)

add_example(example_close close.cpp)
run_example(example_close)

add_example(example_move move.cpp)
run_example(example_move)

add_example(example_multithreading multithreading.cpp)

add_example(example_streaming streaming.cpp)
run_example(example_streaming)

add_example(example_multithreading_static_channel multithreading_static_channel.cpp)
run_example(example_multithreading_static_channel)

add_example(example_merge_channels merge_channels.cpp)
run_example(example_merge_channels)

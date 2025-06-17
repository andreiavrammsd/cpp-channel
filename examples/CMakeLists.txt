function(add_example NAME)
    add_executable(${NAME} ${ARGN})

    set_target_warnings(${NAME} PRIVATE)
    target_link_libraries(${NAME} msd_channel)

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${NAME} PRIVATE -fsanitize=thread)
        target_link_options(${NAME} PRIVATE -fsanitize=thread)
    endif()

    add_dependencies(examples ${NAME})
endfunction()

function(run_example NAME)
    add_custom_target(
        run_${NAME}
        COMMAND ${NAME}
        DEPENDS ${NAME}
        COMMENT "Running example: ${NAME}")

    add_dependencies(run_examples run_${NAME})
endfunction()

add_custom_target(examples)
add_custom_target(run_examples)

# Examples
add_example(example_close close.cpp)
run_example(example_close)

add_example(example_move move.cpp)
run_example(example_move)

add_example(example_multithreading multithreading.cpp)

add_example(example_streaming streaming.cpp)
run_example(example_streaming)

add_example(example_multithreading_static_channel multithreading_static_channel.cpp)
run_example(example_multithreading_static_channel)

add_example(example_concurrent_map_filter concurrent_map_filter.cpp)
run_example(example_concurrent_map_filter)

add_example(example_semaphore semaphore.cpp)
run_example(example_semaphore)

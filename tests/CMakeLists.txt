# Testing framework
if(MSVC)
    option(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
endif()

include(FetchContent)

if(NOT googletest_POPULATED)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
                                        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(googletest)
endif()

# Test macro
function(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})

    set_target_warnings(${TESTNAME} PRIVATE)
    target_link_libraries(${TESTNAME} msd_channel gtest gtest_main)

    if(CPP_CHANNEL_COVERAGE)
        target_compile_options(${TESTNAME} PRIVATE --coverage)
        target_link_libraries(${TESTNAME} -lgcov)

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(COVERAGE_FLAGS -fprofile-instr-generate -fcoverage-mapping)
            target_compile_options(${TESTNAME} PRIVATE ${COVERAGE_FLAGS})
            target_link_libraries(${TESTNAME} ${COVERAGE_FLAGS})
        endif()
    endif()

    set(CPP_CHANNEL_SANITIZER_FLAGS "")
    if(CPP_CHANNEL_SANITIZERS)
        set(SANITIZERS -fsanitize=address -fno-sanitize-recover=address -fsanitize=undefined
                       -fno-sanitize-recover=undefined)

        target_compile_options(${TESTNAME} PRIVATE ${SANITIZERS})
        target_link_options(${TESTNAME} PRIVATE ${SANITIZERS})
    endif()

    if(CPP_CHANNEL_SANITIZE_THREADS)
        target_compile_options(${TESTNAME} PRIVATE -fsanitize=thread)
        target_link_options(${TESTNAME} PRIVATE -fsanitize=thread)
    endif()

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_tests_properties(${TESTNAME} PROPERTIES LABELS "channel_tests")

    add_dependencies(channel_tests ${TESTNAME})
endfunction()

add_custom_target(channel_tests)

# Tests
package_add_test(channel_test channel_test.cpp)
package_add_test(blocking_iterator_test blocking_iterator_test.cpp)
package_add_test(storage_test storage_test.cpp)

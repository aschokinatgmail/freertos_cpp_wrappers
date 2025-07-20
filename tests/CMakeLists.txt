# Test CMakeLists.txt

# =============================================================================
# STATIC ANALYSIS CONFIGURATION (clang-tidy)
# =============================================================================

# Option to enable/disable clang-tidy static analysis
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis for test targets" ON)

# Option to treat clang-tidy warnings as errors (useful for CI)
option(CLANG_TIDY_WARNINGS_AS_ERRORS "Treat clang-tidy warnings as errors" ON)

if(ENABLE_CLANG_TIDY)
    # Find clang-tidy executable
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        
        # Define clang-tidy checks
        # To extend or modify the check set, update the list below:
        # - cppcoreguidelines-*: C++ Core Guidelines compliance
        # - cert-*: CERT C++ Secure Coding Standard
        # - google-*: Google C++ Style Guide compliance  
        # - hicpp-*: High Integrity C++ Coding Standard
        # Add additional check patterns with commas, or exclude specific checks with '-pattern'
        # For faster builds during development, consider using a subset like: "readability-*,performance-*"
        set(CLANG_TIDY_CHECKS
            "cppcoreguidelines-*,cert-*,google-*,hicpp-*"
        )
        
        # Configure clang-tidy to treat warnings as errors for CI-quality enforcement
        # Note: Warnings-as-errors can be disabled during development by setting CLANG_TIDY_WARNINGS_AS_ERRORS=OFF
        if(CLANG_TIDY_WARNINGS_AS_ERRORS)
            set(CMAKE_CXX_CLANG_TIDY 
                ${CLANG_TIDY_EXE};
                --checks=${CLANG_TIDY_CHECKS};
                --warnings-as-errors=*;
                --quiet;
            )
            message(STATUS "clang-tidy warnings will be treated as errors")
        else()
            set(CMAKE_CXX_CLANG_TIDY 
                ${CLANG_TIDY_EXE};
                --checks=${CLANG_TIDY_CHECKS};
                --quiet;
            )
            message(STATUS "clang-tidy warnings will NOT be treated as errors")
        endif()
        
        message(STATUS "clang-tidy enabled with checks: ${CLANG_TIDY_CHECKS}")
    else()
        message(WARNING "clang-tidy not found! Static analysis will be skipped. Install clang-tidy to enable static analysis.")
    endif()
else()
    message(STATUS "clang-tidy disabled via ENABLE_CLANG_TIDY=OFF")
endif()

# =============================================================================
# END STATIC ANALYSIS CONFIGURATION  
# =============================================================================

# Create FreeRTOS mock library first
add_library(freertos_mocks
    mocks/freertos_mocks.cpp
)
target_include_directories(freertos_mocks PUBLIC mocks)

# Find system GoogleTest libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST REQUIRED gtest_main)
pkg_check_modules(GMOCK REQUIRED gmock_main)

# Link GoogleTest to mock library
target_link_libraries(freertos_mocks PUBLIC ${GMOCK_LIBRARIES})
target_compile_options(freertos_mocks PUBLIC ${GMOCK_CFLAGS_OTHER})

# Add the main test executables
add_executable(
    test_freertos_task
    test_freertos_task.cpp
)

add_executable(
    test_freertos_semaphore
    test_freertos_semaphore.cpp
)

add_executable(
    test_freertos_queue
    test_freertos_queue.cpp
)

add_executable(
    test_freertos_event_group
    test_freertos_event_group.cpp
)

add_executable(
    test_freertos_stream_buffer
    test_freertos_stream_buffer.cpp
)

add_executable(
    test_freertos_message_buffer
    test_freertos_message_buffer.cpp
)

target_link_libraries(
    test_freertos_task
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_semaphore
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_queue
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_event_group
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_stream_buffer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_message_buffer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    pthread
)

target_compile_options(test_freertos_task PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_semaphore PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_queue PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_event_group PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_stream_buffer PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_message_buffer PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_freertos_task)
gtest_discover_tests(test_freertos_semaphore)
gtest_discover_tests(test_freertos_queue)
gtest_discover_tests(test_freertos_event_group)
gtest_discover_tests(test_freertos_stream_buffer)
gtest_discover_tests(test_freertos_message_buffer)

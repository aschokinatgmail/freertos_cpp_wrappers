# Test CMakeLists.txt

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

target_compile_options(test_freertos_task PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_semaphore PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_queue PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_event_group PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
target_compile_options(test_freertos_stream_buffer PRIVATE ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_freertos_task)
gtest_discover_tests(test_freertos_semaphore)
gtest_discover_tests(test_freertos_queue)
gtest_discover_tests(test_freertos_event_group)
gtest_discover_tests(test_freertos_stream_buffer)
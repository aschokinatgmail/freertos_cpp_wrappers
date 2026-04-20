# Test CMakeLists.txt

# Create FreeRTOS mock library first
add_library(freertos_mocks
    mocks/freertos_mocks.cpp
    mocks/enhanced_timer_mocks.cpp
)
target_include_directories(freertos_mocks PUBLIC mocks)

# Find GoogleTest using CMake config first, fallback to PkgConfig
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTEST REQUIRED gtest_main)
    pkg_check_modules(GMOCK REQUIRED gmock_main)
    set(GTEST_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})
    set(GTEST_COMPILE_OPTIONS ${GTEST_CFLAGS_OTHER} ${GMOCK_CFLAGS_OTHER})
else()
    set(GTEST_LIBRARIES GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    set(GTEST_COMPILE_OPTIONS "")
endif()

# Link GoogleTest to mock library
target_link_libraries(freertos_mocks PUBLIC ${GTEST_LIBRARIES})
if(GTEST_COMPILE_OPTIONS)
    target_compile_options(freertos_mocks PUBLIC ${GTEST_COMPILE_OPTIONS})
endif()

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

add_executable(
    test_freertos_sw_timer
    test_freertos_sw_timer.cpp
)

add_executable(
    test_enhanced_multitasking
    test_enhanced_multitasking.cpp
)

add_executable(
    test_freertos_semaphore_stl
    test_freertos_semaphore_stl.cpp
)

add_executable(
    test_enhanced_timeout_precision
    test_enhanced_timeout_precision.cpp
)

add_executable(
    test_enhanced_cpp17_features
    test_enhanced_cpp17_features.cpp
)

add_executable(
    test_enhanced_freertos_sw_timer
    test_enhanced_freertos_sw_timer.cpp
)

add_executable(
    test_enhanced_debug
    test_enhanced_debug.cpp
)

add_executable(
    test_freertos_config
    test_freertos_config.cpp
)

add_executable(
    test_freertos_span
    test_freertos_span.cpp
)

add_executable(
    test_freertos_expected
    test_freertos_expected.cpp
)

add_executable(
    test_freertos_external_allocator
    test_freertos_external_allocator.cpp
)

add_executable(
    test_freertos_strong_types
    test_freertos_strong_types.cpp
)

add_executable(
    test_freertos_expected_api
    test_freertos_expected_api.cpp
)

add_executable(
    test_freertos_indexed_notifications
    test_freertos_indexed_notifications.cpp
)

add_executable(
    test_freertos_misc_api
    test_freertos_misc_api.cpp
)

add_executable(
    test_freertos_smp_affinity
    test_freertos_smp_affinity.cpp
)

add_executable(
    test_freertos_queue_set
    test_freertos_queue_set.cpp
)

add_executable(
    test_freertos_dual_api
    test_freertos_dual_api.cpp
)

add_executable(
    test_freertos_thread_safety
    test_freertos_thread_safety.cpp
)

add_executable(
    test_freertos_move_semantics
    test_freertos_move_semantics.cpp
)

add_executable(
    test_freertos_coverage_semaphores
    test_freertos_coverage_semaphores.cpp
)

add_executable(
    test_freertos_coverage_api
    test_freertos_coverage_api.cpp
)

add_executable(
    test_freertos_coverage_branches
    test_freertos_coverage_branches.cpp
)

add_executable(
    test_freertos_coverage_gaps
    test_freertos_coverage_gaps.cpp
)

add_executable(
    test_freertos_expected_monadic
    test_freertos_expected_monadic.cpp
)

add_executable(
    test_freertos_coverage_branches2
    test_freertos_coverage_branches2.cpp
)

add_executable(
    test_freertos_coverage_sa_ns
    test_freertos_coverage_sa_ns.cpp
)

add_executable(
    test_freertos_coverage_faults
    test_freertos_coverage_faults.cpp
)

add_executable(
    test_freertos_isr_result
    test_freertos_isr_result.cpp
)

add_executable(
    test_freertos_concurrency
    test_freertos_concurrency.cpp
)

target_link_libraries(
    test_freertos_task
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_semaphore
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_queue
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_event_group
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_stream_buffer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_message_buffer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_sw_timer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_enhanced_multitasking
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_semaphore_stl
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_enhanced_timeout_precision
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_enhanced_cpp17_features
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_enhanced_freertos_sw_timer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_enhanced_debug
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_config
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_span
    freertos_cpp_wrappers
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_expected
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_external_allocator
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_strong_types
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_expected_api
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_indexed_notifications
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_misc_api
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_smp_affinity
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_queue_set
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_dual_api
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_thread_safety
    freertos_cpp_wrappers
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_move_semantics
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_semaphores
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_api
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_branches
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_gaps
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_expected_monadic
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_branches2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_sa_ns
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_faults
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_isr_result
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_concurrency
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

# Apply compile options if using PkgConfig
if(GTEST_COMPILE_OPTIONS)
    target_compile_options(test_freertos_task PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_semaphore PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_queue PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_event_group PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_stream_buffer PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_message_buffer PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_sw_timer PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_enhanced_multitasking PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_semaphore_stl PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_enhanced_timeout_precision PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_enhanced_cpp17_features PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_enhanced_freertos_sw_timer PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_enhanced_debug PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_config PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_span PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_expected PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_external_allocator PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_strong_types PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_expected_api PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_indexed_notifications PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_misc_api PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_smp_affinity PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_queue_set PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_dual_api PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_thread_safety PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_move_semantics PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_semaphores PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_api PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_branches PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_gaps PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_expected_monadic PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_branches2 PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_sa_ns PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_faults PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_isr_result PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_concurrency PRIVATE ${GTEST_COMPILE_OPTIONS})
endif()

# Add tests - run complete test executables to ensure proper failure detection
# Using add_test instead of gtest_discover_tests to avoid individual test isolation issues
add_test(NAME test_freertos_task COMMAND test_freertos_task)
add_test(NAME test_freertos_semaphore COMMAND test_freertos_semaphore)
add_test(NAME test_freertos_queue COMMAND test_freertos_queue)
add_test(NAME test_freertos_event_group COMMAND test_freertos_event_group)
add_test(NAME test_freertos_stream_buffer COMMAND test_freertos_stream_buffer)
add_test(NAME test_freertos_message_buffer COMMAND test_freertos_message_buffer)
add_test(NAME test_freertos_sw_timer COMMAND test_freertos_sw_timer)
add_test(NAME test_enhanced_multitasking COMMAND test_enhanced_multitasking)
add_test(NAME test_freertos_semaphore_stl COMMAND test_freertos_semaphore_stl)
add_test(NAME test_enhanced_timeout_precision COMMAND test_enhanced_timeout_precision)
add_test(NAME test_enhanced_cpp17_features COMMAND test_enhanced_cpp17_features)
add_test(NAME test_enhanced_freertos_sw_timer COMMAND test_enhanced_freertos_sw_timer)
add_test(NAME test_enhanced_debug COMMAND test_enhanced_debug)
add_test(NAME test_freertos_config COMMAND test_freertos_config)
add_test(NAME test_freertos_span COMMAND test_freertos_span)
add_test(NAME test_freertos_expected COMMAND test_freertos_expected)
add_test(NAME test_freertos_external_allocator COMMAND test_freertos_external_allocator)
add_test(NAME test_freertos_strong_types COMMAND test_freertos_strong_types)
add_test(NAME test_freertos_expected_api COMMAND test_freertos_expected_api)
add_test(NAME test_freertos_indexed_notifications COMMAND test_freertos_indexed_notifications)
add_test(NAME test_freertos_misc_api COMMAND test_freertos_misc_api)
add_test(NAME test_freertos_smp_affinity COMMAND test_freertos_smp_affinity)
add_test(NAME test_freertos_queue_set COMMAND test_freertos_queue_set)
add_test(NAME test_freertos_dual_api COMMAND test_freertos_dual_api)
add_test(NAME test_freertos_thread_safety COMMAND test_freertos_thread_safety)
add_test(NAME test_freertos_move_semantics COMMAND test_freertos_move_semantics)
add_test(NAME test_freertos_coverage_semaphores COMMAND test_freertos_coverage_semaphores)
add_test(NAME test_freertos_coverage_api COMMAND test_freertos_coverage_api)
add_test(NAME test_freertos_coverage_branches COMMAND test_freertos_coverage_branches)
add_test(NAME test_freertos_coverage_gaps COMMAND test_freertos_coverage_gaps)
add_test(NAME test_freertos_expected_monadic COMMAND test_freertos_expected_monadic)
add_test(NAME test_freertos_coverage_branches2 COMMAND test_freertos_coverage_branches2)
add_test(NAME test_freertos_coverage_sa_ns COMMAND test_freertos_coverage_sa_ns)
add_test(NAME test_freertos_coverage_faults COMMAND test_freertos_coverage_faults)
add_test(NAME test_freertos_isr_result COMMAND test_freertos_isr_result)
add_test(NAME test_freertos_concurrency COMMAND test_freertos_concurrency)

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
    test_freertos_coverage_semaphores2
    test_freertos_coverage_semaphores2.cpp
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
    test_freertos_coverage_branches3
    test_freertos_coverage_branches3.cpp
)

add_executable(
    test_freertos_coverage_misc_branches
    test_freertos_coverage_misc_branches.cpp
)

add_executable(
    test_freertos_isr_result
    test_freertos_isr_result.cpp
)

add_executable(
    test_freertos_concurrency
    test_freertos_concurrency.cpp
)

add_executable(
    test_freertos_coverage_ex_variants
    test_freertos_coverage_ex_variants.cpp
)

add_executable(
    test_freertos_coverage_isr
    test_freertos_coverage_isr.cpp
)

add_executable(
    test_freertos_coverage_task_timer_isr
    test_freertos_coverage_task_timer_isr.cpp
)

add_executable(
    test_freertos_coverage_sa
    test_freertos_coverage_sa.cpp
)

add_executable(
    test_freertos_coverage_external_allocator2
    test_freertos_coverage_external_allocator2.cpp
)

add_executable(
    test_freertos_clock
    test_freertos_clock.cpp
)

add_executable(
    test_freertos_concepts
    test_freertos_concepts.cpp
)

add_executable(
    test_freertos_config_kconfig
    test_freertos_config_kconfig.cpp
)

add_executable(
    test_freertos_atomic_wait
    test_freertos_atomic_wait.cpp
)
target_compile_definitions(test_freertos_atomic_wait PRIVATE
    FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=1
    FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=3
)

add_executable(
    test_freertos_gthr
    test_freertos_gthr.cpp
)

add_executable(
    test_freertos_external_threading
    test_freertos_external_threading.cpp
)

add_executable(
    test_freertos_gthr_impl
    test_freertos_gthr_impl.cpp
)

add_executable(
    test_freertos_external_threading_impl
    test_freertos_external_threading_impl.cpp
)

add_executable(
    test_freertos_coverage_stream_message
    test_freertos_coverage_stream_message.cpp
)

add_executable(
    test_freertos_coverage_stream_message2
    test_freertos_coverage_stream_message2.cpp
)

add_executable(
    test_freertos_coverage_external_allocator3
    test_freertos_coverage_external_allocator3.cpp
)

add_executable(
    test_freertos_coverage_queue_timer_task
    test_freertos_coverage_queue_timer_task.cpp
)

add_executable(
    test_freertos_coverage_event_group2
    test_freertos_coverage_event_group2.cpp
)

add_executable(
    test_freertos_coverage_alloc_swap
    test_freertos_coverage_alloc_swap.cpp
)

add_executable(
    test_freertos_coverage_semaphores3
    test_freertos_coverage_semaphores3.cpp
)

add_executable(
    test_freertos_coverage_semaphores4
    test_freertos_coverage_semaphores4.cpp
)

add_executable(
    test_freertos_coverage_queue2
    test_freertos_coverage_queue2.cpp
)

add_executable(
    test_freertos_coverage_semaphores5
    test_freertos_coverage_semaphores5.cpp
)

add_executable(
    test_freertos_coverage_stream_buffer3
    test_freertos_coverage_stream_buffer3.cpp
)

add_executable(
    test_freertos_coverage_external_allocator4
    test_freertos_coverage_external_allocator4.cpp
)

add_executable(
    test_freertos_coverage_task2
    test_freertos_coverage_task2.cpp
)

add_executable(
    test_freertos_coverage_expected2
    test_freertos_coverage_expected2.cpp
)

add_executable(
    test_freertos_coverage_timer2
    test_freertos_coverage_timer2.cpp
)

add_executable(
    test_freertos_coverage_expected3
    test_freertos_coverage_expected3.cpp
)

add_executable(
    test_freertos_coverage_branches4
    test_freertos_coverage_branches4.cpp
)

add_executable(
    test_freertos_coverage_exception_safety
    test_freertos_coverage_exception_safety.cpp
)

add_executable(
    test_freertos_stream_buffer_callbacks
    test_freertos_stream_buffer_callbacks.cpp
)

add_executable(
    test_freertos_task_join
    test_freertos_task_join.cpp
)

add_executable(
    test_freertos_stream_batching_buffer
    test_freertos_stream_batching_buffer.cpp
)

add_executable(
    test_freertos_fixed_function
    test_freertos_fixed_function.cpp
)

add_executable(
    test_freertos_isr_context
    test_freertos_isr_context.cpp
)

add_executable(
    test_freertos_once_flag
    test_freertos_once_flag.cpp
)

add_executable(
    test_freertos_pend_call
    test_freertos_pend_call.cpp
)

add_executable(
    test_freertos_shared_data
    test_freertos_shared_data.cpp
)

add_executable(
    test_freertos_claim
    test_freertos_claim.cpp
)

add_executable(
    test_freertos_heap
    test_freertos_heap.cpp
)

add_executable(
    test_freertos_latch
    test_freertos_latch.cpp
)

add_executable(
    test_freertos_shared_mutex
    test_freertos_shared_mutex.cpp
)

add_executable(
    test_freertos_atomic
    test_freertos_atomic.cpp
)
target_compile_definitions(test_freertos_atomic PRIVATE
    FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=1
    FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=3
)

add_executable(
    test_freertos_lock
    test_freertos_lock.cpp
)

add_executable(
    test_freertos_condition_variable
    test_freertos_condition_variable.cpp
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
    test_freertos_coverage_semaphores2
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
    test_freertos_coverage_branches3
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_misc_branches
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

target_link_libraries(
    test_freertos_coverage_ex_variants
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_isr
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_task_timer_isr
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_sa
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_external_allocator2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_clock
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_concepts
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_config_kconfig
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_atomic_wait
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_gthr
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_external_threading
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_gthr_impl
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_external_threading_impl
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_stream_message
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_stream_message2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_external_allocator3
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_queue_timer_task
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_event_group2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_alloc_swap
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_semaphores3
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_semaphores4
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_queue2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_semaphores5
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_stream_buffer3
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_external_allocator4
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_task2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_expected2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_timer2
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_expected3
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_exception_safety
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_coverage_branches4
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_stream_buffer_callbacks
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_task_join
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_stream_batching_buffer
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_fixed_function
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_isr_context
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_once_flag
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_heap
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_latch
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_shared_mutex
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_atomic
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_pend_call
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_shared_data
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_claim
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_lock
    freertos_cpp_wrappers
    freertos_mocks
    ${GTEST_LIBRARIES}
    pthread
)

target_link_libraries(
    test_freertos_condition_variable
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
    target_compile_options(test_freertos_coverage_semaphores2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_api PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_branches PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_gaps PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_expected_monadic PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_branches2 PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_sa_ns PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_coverage_faults PRIVATE ${GTEST_COMPILE_OPTIONS})
target_compile_options(test_freertos_isr_result PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_concurrency PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_ex_variants PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_isr PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_task_timer_isr PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_sa PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_external_allocator2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_clock PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_concepts PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_config_kconfig PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_atomic_wait PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_gthr PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_external_threading PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_gthr_impl PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_external_threading_impl PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_stream_message PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_stream_message2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_external_allocator3 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_queue_timer_task PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_event_group2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_alloc_swap PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_semaphores3 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_semaphores4 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_queue2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_semaphores5 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_stream_buffer3 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_external_allocator4 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_task2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_branches3 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_misc_branches PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_expected2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_timer2 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_expected3 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_branches4 PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_coverage_exception_safety PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_stream_buffer_callbacks PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_stream_batching_buffer PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_task_join PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_fixed_function PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_isr_context PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_once_flag PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_heap PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_pend_call PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_shared_data PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_claim PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_latch PRIVATE ${GTEST_COMPILE_OPTIONS})
    target_compile_options(test_freertos_shared_mutex PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_atomic PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_lock PRIVATE ${GTEST_COMPILE_OPTIONS})
     target_compile_options(test_freertos_condition_variable PRIVATE ${GTEST_COMPILE_OPTIONS})
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
add_test(NAME test_freertos_coverage_semaphores2 COMMAND test_freertos_coverage_semaphores2)
add_test(NAME test_freertos_coverage_api COMMAND test_freertos_coverage_api)
add_test(NAME test_freertos_coverage_branches COMMAND test_freertos_coverage_branches)
add_test(NAME test_freertos_coverage_gaps COMMAND test_freertos_coverage_gaps)
add_test(NAME test_freertos_expected_monadic COMMAND test_freertos_expected_monadic)
add_test(NAME test_freertos_coverage_branches2 COMMAND test_freertos_coverage_branches2)
add_test(NAME test_freertos_coverage_sa_ns COMMAND test_freertos_coverage_sa_ns)
add_test(NAME test_freertos_coverage_faults COMMAND test_freertos_coverage_faults)
add_test(NAME test_freertos_isr_result COMMAND test_freertos_isr_result)
add_test(NAME test_freertos_concurrency COMMAND test_freertos_concurrency)
add_test(NAME test_freertos_coverage_ex_variants COMMAND test_freertos_coverage_ex_variants)
add_test(NAME test_freertos_coverage_isr COMMAND test_freertos_coverage_isr)
add_test(NAME test_freertos_coverage_task_timer_isr COMMAND test_freertos_coverage_task_timer_isr)
add_test(NAME test_freertos_coverage_sa COMMAND test_freertos_coverage_sa)
add_test(NAME test_freertos_coverage_external_allocator2 COMMAND test_freertos_coverage_external_allocator2)
add_test(NAME test_freertos_concepts COMMAND test_freertos_concepts)
add_test(NAME test_freertos_clock COMMAND test_freertos_clock)
add_test(NAME test_freertos_config_kconfig COMMAND test_freertos_config_kconfig)
add_test(NAME test_freertos_atomic_wait COMMAND test_freertos_atomic_wait)
add_test(NAME test_freertos_gthr COMMAND test_freertos_gthr)
add_test(NAME test_freertos_external_threading COMMAND test_freertos_external_threading)
add_test(NAME test_freertos_gthr_impl COMMAND test_freertos_gthr_impl)
add_test(NAME test_freertos_external_threading_impl COMMAND test_freertos_external_threading_impl)
add_test(NAME test_freertos_coverage_stream_message COMMAND test_freertos_coverage_stream_message)
add_test(NAME test_freertos_coverage_stream_message2 COMMAND test_freertos_coverage_stream_message2)
add_test(NAME test_freertos_coverage_external_allocator3 COMMAND test_freertos_coverage_external_allocator3)
add_test(NAME test_freertos_coverage_queue_timer_task COMMAND test_freertos_coverage_queue_timer_task)
add_test(NAME test_freertos_coverage_event_group2 COMMAND test_freertos_coverage_event_group2)
add_test(NAME test_freertos_coverage_alloc_swap COMMAND test_freertos_coverage_alloc_swap)
add_test(NAME test_freertos_coverage_semaphores3 COMMAND test_freertos_coverage_semaphores3)
add_test(NAME test_freertos_coverage_semaphores4 COMMAND test_freertos_coverage_semaphores4)
add_test(NAME test_freertos_coverage_queue2 COMMAND test_freertos_coverage_queue2)
add_test(NAME test_freertos_coverage_semaphores5 COMMAND test_freertos_coverage_semaphores5)
add_test(NAME test_freertos_coverage_stream_buffer3 COMMAND test_freertos_coverage_stream_buffer3)
add_test(NAME test_freertos_coverage_external_allocator4 COMMAND test_freertos_coverage_external_allocator4)
add_test(NAME test_freertos_coverage_task2 COMMAND test_freertos_coverage_task2)
add_test(NAME test_freertos_coverage_branches3 COMMAND test_freertos_coverage_branches3)
add_test(NAME test_freertos_coverage_misc_branches COMMAND test_freertos_coverage_misc_branches)
add_test(NAME test_freertos_coverage_expected2 COMMAND test_freertos_coverage_expected2)

add_test(NAME test_freertos_coverage_timer2 COMMAND test_freertos_coverage_timer2)
add_test(NAME test_freertos_coverage_expected3 COMMAND test_freertos_coverage_expected3)
add_test(NAME test_freertos_coverage_branches4 COMMAND test_freertos_coverage_branches4)
add_test(NAME test_freertos_coverage_exception_safety COMMAND test_freertos_coverage_exception_safety)

add_test(NAME test_freertos_stream_buffer_callbacks COMMAND test_freertos_stream_buffer_callbacks)
add_test(NAME test_freertos_task_join COMMAND test_freertos_task_join)
add_test(NAME test_freertos_stream_batching_buffer COMMAND test_freertos_stream_batching_buffer)
add_test(NAME test_freertos_isr_context COMMAND test_freertos_isr_context)
add_test(NAME test_freertos_once_flag COMMAND test_freertos_once_flag)
add_test(NAME test_freertos_fixed_function COMMAND test_freertos_fixed_function)
add_test(NAME test_freertos_heap COMMAND test_freertos_heap)
add_test(NAME test_freertos_pend_call COMMAND test_freertos_pend_call)
add_test(NAME test_freertos_shared_data COMMAND test_freertos_shared_data)
add_test(NAME test_freertos_claim COMMAND test_freertos_claim)
add_test(NAME test_freertos_latch COMMAND test_freertos_latch)
add_test(NAME test_freertos_shared_mutex COMMAND test_freertos_shared_mutex)
add_test(NAME test_freertos_atomic COMMAND test_freertos_atomic)
add_test(NAME test_freertos_lock COMMAND test_freertos_lock)
add_test(NAME test_freertos_condition_variable COMMAND test_freertos_condition_variable)

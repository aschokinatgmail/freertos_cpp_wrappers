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
endif()

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_freertos_task)
gtest_discover_tests(test_freertos_semaphore)
gtest_discover_tests(test_freertos_queue)
gtest_discover_tests(test_freertos_event_group)
gtest_discover_tests(test_freertos_stream_buffer)
gtest_discover_tests(test_freertos_message_buffer)
gtest_discover_tests(test_freertos_sw_timer)
gtest_discover_tests(test_enhanced_multitasking)
gtest_discover_tests(test_freertos_semaphore_stl)
gtest_discover_tests(test_enhanced_timeout_precision)
gtest_discover_tests(test_enhanced_cpp17_features)
gtest_discover_tests(test_enhanced_freertos_sw_timer)

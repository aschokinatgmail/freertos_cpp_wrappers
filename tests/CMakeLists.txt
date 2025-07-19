cmake_minimum_required(VERSION 3.14)

# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add freertos stub headers directory
add_library(freertos_stubs STATIC freertos_stubs/freertos_stubs.cpp)
target_include_directories(freertos_stubs PUBLIC freertos_stubs)

# Test executable for each module
add_executable(test_freertos test_freertos.cpp)
target_link_libraries(test_freertos 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos PRIVATE ../include)
target_compile_options(test_freertos PRIVATE -fpermissive)

add_executable(test_freertos_event_group test_freertos_event_group.cpp)
target_link_libraries(test_freertos_event_group 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_event_group PRIVATE ../include)

add_executable(test_freertos_message_buffer test_freertos_message_buffer.cpp)
target_link_libraries(test_freertos_message_buffer 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_message_buffer PRIVATE ../include)

add_executable(test_freertos_queue test_freertos_queue.cpp)
target_link_libraries(test_freertos_queue 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_queue PRIVATE ../include)

add_executable(test_freertos_semaphore test_freertos_semaphore.cpp)
target_link_libraries(test_freertos_semaphore 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_semaphore PRIVATE ../include)

add_executable(test_freertos_stream_buffer test_freertos_stream_buffer.cpp)
target_link_libraries(test_freertos_stream_buffer 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_stream_buffer PRIVATE ../include)

add_executable(test_freertos_sw_timer test_freertos_sw_timer.cpp)
target_link_libraries(test_freertos_sw_timer 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_sw_timer PRIVATE ../include)

add_executable(test_freertos_task test_freertos_task.cpp)
target_link_libraries(test_freertos_task 
    gtest_main 
    freertos_stubs)
target_include_directories(test_freertos_task PRIVATE ../include)
target_compile_options(test_freertos_task PRIVATE -fpermissive)

# Add all tests to CTest
include(GoogleTest)
gtest_discover_tests(test_freertos)
gtest_discover_tests(test_freertos_event_group)
gtest_discover_tests(test_freertos_message_buffer)
gtest_discover_tests(test_freertos_queue)
gtest_discover_tests(test_freertos_semaphore)
gtest_discover_tests(test_freertos_stream_buffer)
gtest_discover_tests(test_freertos_sw_timer)
gtest_discover_tests(test_freertos_task)
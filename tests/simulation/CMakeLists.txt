if(NOT ENABLE_SIMULATION)
    return()
endif()

set(FREERTOS_KERNEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/FreeRTOS-Kernel)

set(FREERTOS_KERNEL_SOURCES
    ${FREERTOS_KERNEL_DIR}/tasks.c
    ${FREERTOS_KERNEL_DIR}/queue.c
    ${FREERTOS_KERNEL_DIR}/list.c
    ${FREERTOS_KERNEL_DIR}/timers.c
    ${FREERTOS_KERNEL_DIR}/event_groups.c
    ${FREERTOS_KERNEL_DIR}/stream_buffer.c
)

set(FREERTOS_POSIX_PORT_SOURCES
    ${FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Posix/port.c
    ${FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
    ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

add_library(freertos_kernel_sim STATIC
    ${FREERTOS_KERNEL_SOURCES}
    ${FREERTOS_POSIX_PORT_SOURCES}
)

target_include_directories(freertos_kernel_sim PUBLIC
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Posix
    ${FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(freertos_kernel_sim PRIVATE
    -Wno-unused-parameter
    -Wno-old-style-cast
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-pedantic
    -Wno-missing-field-initializers
    -Wno-cast-qual
    -Wno-missing-braces
    -Wno-undef
    -fno-exceptions
)

target_compile_definitions(freertos_kernel_sim PUBLIC
    SIMULATION
)

target_link_libraries(freertos_kernel_sim PUBLIC
    pthread
)
if(NOT APPLE)
    target_link_libraries(freertos_kernel_sim PUBLIC rt)
endif()

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTEST REQUIRED gtest_main)
    set(GTEST_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})
else()
    set(GTEST_LIBRARIES GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
endif()

set(SIM_COMPILE_OPTIONS
    -Wno-old-style-cast
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-pedantic
    -Wno-undef
    -Wno-missing-field-initializers
    -Wno-cast-qual
    -Wno-missing-braces
    -Wno-shadow
    -Wno-unused-variable
    -fno-exceptions
)

# --- Smoke tests (raw C API) ---
add_executable(simulation_smoke
    test_sim_smoke.cpp
)

target_link_libraries(simulation_smoke
    freertos_kernel_sim
    ${GTEST_LIBRARIES}
    pthread
)

target_compile_options(simulation_smoke PRIVATE
    ${SIM_COMPILE_OPTIONS}
)

add_test(
    NAME simulation_smoke
    COMMAND simulation_smoke
)

set_tests_properties(simulation_smoke PROPERTIES
    TIMEOUT 60
    LABELS "simulation"
)

# --- System tests (C++ wrappers) ---
add_executable(simulation_system
    test_sim_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/freertos_task.cc
)

target_include_directories(simulation_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_BINARY_DIR}/../../include
)

target_link_libraries(simulation_system
    freertos_kernel_sim
    ${GTEST_LIBRARIES}
    pthread
)

target_compile_options(simulation_system PRIVATE
    ${SIM_COMPILE_OPTIONS}
)

add_test(
    NAME simulation_system
    COMMAND simulation_system
)

set_tests_properties(simulation_system PROPERTIES
    TIMEOUT 120
    LABELS "simulation"
)

# --- Helper function for simulation test targets ---
function(add_sim_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/freertos_task.cc
    )
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_BINARY_DIR}/../../include
    )
    target_link_libraries(${TEST_NAME}
        freertos_kernel_sim
        ${GTEST_LIBRARIES}
        pthread
    )
    target_compile_options(${TEST_NAME} PRIVATE
        ${SIM_COMPILE_OPTIONS}
    )
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
    )
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 120
        LABELS "simulation"
    )
endfunction()

# --- Category test suites (C++ wrappers) ---
add_sim_test(simulation_semaphore test_sim_semaphore.cpp)
add_sim_test(simulation_mutex test_sim_mutex.cpp)
add_sim_test(simulation_queue test_sim_queue.cpp)
add_sim_test(simulation_stream_buffer test_sim_stream_buffer.cpp)
add_sim_test(simulation_message_buffer test_sim_message_buffer.cpp)
add_sim_test(simulation_timer test_sim_timer.cpp)
add_sim_test(simulation_event_group test_sim_event_group.cpp)
add_sim_test(simulation_task test_sim_task.cpp)
add_sim_test(simulation_move_swap test_sim_move_swap.cpp)
add_sim_test(simulation_isr test_sim_isr.cpp)
add_sim_test(simulation_concurrency test_sim_concurrency.cpp)
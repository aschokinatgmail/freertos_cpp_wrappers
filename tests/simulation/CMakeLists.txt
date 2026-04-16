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
    rt
)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTEST REQUIRED gtest_main)
    set(GTEST_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})
else()
    set(GTEST_LIBRARIES GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
endif()

add_executable(simulation_smoke
    test_sim_smoke.cpp
)

target_link_libraries(simulation_smoke
    freertos_kernel_sim
    ${GTEST_LIBRARIES}
    pthread
)

target_compile_options(simulation_smoke PRIVATE
    -Wno-old-style-cast
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-pedantic
    -Wno-undef
    -fno-exceptions
)

add_test(
    NAME simulation_smoke
    COMMAND simulation_smoke
)

set_tests_properties(simulation_smoke PROPERTIES
    TIMEOUT 60
    LABELS "simulation"
)
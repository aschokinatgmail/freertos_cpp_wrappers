if(NOT ENABLE_QEMU_TEST)
    return()
endif()

if(NOT CMAKE_C_COMPILER MATCHES "arm-none-eabi")
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(FREERTOS_KERNEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/FreeRTOS-Kernel)

set(FREERTOS_KERNEL_SOURCES
    ${FREERTOS_KERNEL_DIR}/tasks.c
    ${FREERTOS_KERNEL_DIR}/queue.c
    ${FREERTOS_KERNEL_DIR}/list.c
    ${FREERTOS_KERNEL_DIR}/timers.c
    ${FREERTOS_KERNEL_DIR}/event_groups.c
    ${FREERTOS_KERNEL_DIR}/stream_buffer.c
)

set(QEMU_C_FLAGS
    -mcpu=cortex-m3 -mthumb
    -ffreestanding -O2 -Wno-unused-parameter
)

set(QEMU_CXX_FLAGS
    -mcpu=cortex-m3 -mthumb
    -ffreestanding -O2 -fno-exceptions -fno-rtti -fno-unwind-tables
    -Wno-old-style-cast -Wno-sign-conversion -Wno-conversion
    -Wno-pedantic -Wno-undef -Wno-missing-field-initializers
    -Wno-cast-qual -Wno-shadow -Wno-unused-parameter -Wno-unused-variable
)

set(QEMU_LINK_FLAGS
    -mcpu=cortex-m3 -mthumb
    --specs=nosys.specs -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    -Wl,--gc-sections
)

set(QEMU_INCLUDES
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM3
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(freertos_kernel_qemu STATIC
    ${FREERTOS_KERNEL_SOURCES}
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM3/port.c
    ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

target_include_directories(freertos_kernel_qemu PUBLIC
    ${QEMU_INCLUDES}
)

target_compile_options(freertos_kernel_qemu PRIVATE
    ${QEMU_C_FLAGS}
)

add_executable(qemu_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.c
)

target_include_directories(qemu_test PRIVATE
    ${QEMU_INCLUDES}
)

target_compile_options(qemu_test PRIVATE
    ${QEMU_C_FLAGS}
)

target_link_options(qemu_test PRIVATE
    ${QEMU_LINK_FLAGS}
)

target_link_libraries(qemu_test PRIVATE
    freertos_kernel_qemu
    c nosys gcc
)

add_custom_command(TARGET qemu_test POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:qemu_test>
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:qemu_test> ${CMAKE_CURRENT_BINARY_DIR}/qemu_test.bin
    COMMENT "qemu_test size and binary:"
)

add_custom_target(qemu_run
    COMMAND ${CMAKE_COMMAND} -E echo "Running FreeRTOS test in QEMU (MPS2-AN385)..."
    COMMAND bash -c "timeout 30 qemu-system-arm -machine mps2-an385 -nographic -kernel $<TARGET_FILE:qemu_test> 2>&1 | tee qemu_test.log && grep -q SUCCESS qemu_test.log"
    DEPENDS qemu_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running FreeRTOS test in QEMU (MPS2-AN385)"
    VERBATIM
)
if(NOT ENABLE_CROSS_COMPILE)
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

set(FREERTOS_ARM_PORT_SOURCES
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c
    ${FREERTOS_KERNEL_DIR}/portable/MemMng/heap_4.c
)

# --- ARM compile flags ---
set(XCOMPILE_C_FLAGS
    -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -ffreestanding -O2 -Wno-unused-parameter
)

set(XCOMPILE_CXX_FLAGS
    -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -ffreestanding -O2 -fno-exceptions -fno-rtti -fno-unwind-tables
    -Wno-old-style-cast -Wno-sign-conversion -Wno-conversion
    -Wno-pedantic -Wno-undef -Wno-missing-field-initializers
    -Wno-cast-qual -Wno-shadow -Wno-unused-parameter -Wno-unused-variable
)

# --- Common include directories ---
set(FREERTOS_INCLUDES
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_BINARY_DIR}/../../include
)

# --- FreeRTOS kernel library for ARM Cortex-M4F ---
add_library(freertos_kernel_arm STATIC
    ${FREERTOS_KERNEL_SOURCES}
    ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

target_include_directories(freertos_kernel_arm PUBLIC
    ${FREERTOS_INCLUDES}
)

target_compile_options(freertos_kernel_arm PRIVATE
    ${XCOMPILE_C_FLAGS}
)

# --- ARM bare-metal binary (kernel + startup) ---
add_executable(xcompile_baremetal
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.c
)

target_include_directories(xcompile_baremetal PRIVATE
    ${FREERTOS_INCLUDES}
)

target_compile_options(xcompile_baremetal PRIVATE
    ${XCOMPILE_C_FLAGS}
)

target_link_options(xcompile_baremetal PRIVATE
    -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -nostdlib -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    -Wl,--gc-sections
)

target_link_libraries(xcompile_baremetal PRIVATE
    freertos_kernel_arm
)

add_custom_command(TARGET xcompile_baremetal POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:xcompile_baremetal>
    COMMENT "xcompile_baremetal size:"
)

# --- Compile-only verification using a script ---
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/run_xcompile_check.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run_xcompile_check.sh
    @ONLY
)

add_custom_target(xcompile_gthr
    COMMAND bash ${CMAKE_CURRENT_BINARY_DIR}/run_xcompile_check.sh
        "${CMAKE_CURRENT_SOURCE_DIR}/test_xcompile_gthr.cpp"
        "FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND=1;FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=1;FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY=3;FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL=1;FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD=1;FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX=1"
    COMMENT "gthr-FreeRTOS backend compile-only check for ARM Cortex-M"
    VERBATIM
)

add_custom_target(xcompile_et
    COMMAND bash ${CMAKE_CURRENT_BINARY_DIR}/run_xcompile_check.sh
        "${CMAKE_CURRENT_SOURCE_DIR}/test_xcompile_et.cpp"
        "FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING=1;FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=1;FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY=3;FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL=1;FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD=1;FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX=1"
    COMMENT "libc++ __external_threading backend compile-only check for ARM Cortex-M"
    VERBATIM
)

add_custom_target(xcompile_all
    DEPENDS xcompile_baremetal xcompile_gthr xcompile_et
    COMMENT "All cross-compile verification checks passed for ARM Cortex-M"
)
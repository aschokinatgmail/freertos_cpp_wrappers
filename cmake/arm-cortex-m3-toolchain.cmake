set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(COMMON_ARM_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS_INIT "${COMMON_ARM_FLAGS} -ffreestanding -O2")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_ARM_FLAGS} -ffreestanding -fno-exceptions -fno-rtti -fno-unwind-tables -O2")
set(CMAKE_ASM_FLAGS_INIT "${COMMON_ARM_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_ARM_FLAGS} -nostdlib -Wl,--gc-sections")
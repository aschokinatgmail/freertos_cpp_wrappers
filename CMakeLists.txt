cmake_minimum_required(VERSION 3.14)
project(freertos_cpp_wrappers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GoogleTest
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST REQUIRED gtest_main)
pkg_check_modules(GMOCK REQUIRED gmock_main)

# Enable testing
enable_testing()

# Include directories
include_directories(include)
include_directories(tests/mocks)

# Add the library source files
add_library(freertos_cpp_wrappers 
    src/freertos_task.cc
)

# Add tests subdirectory
add_subdirectory(tests)
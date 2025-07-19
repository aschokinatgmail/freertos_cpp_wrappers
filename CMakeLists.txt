cmake_minimum_required(VERSION 3.14)

project(freertos_cpp_wrappers
    VERSION 0.1.0
    DESCRIPTION "FreeRTOS C++ Wrappers Library"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create interface library for the headers
add_library(freertos_cpp_wrappers INTERFACE)

# Add include directory
target_include_directories(freertos_cpp_wrappers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)
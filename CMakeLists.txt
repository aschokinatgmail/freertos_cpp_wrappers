cmake_minimum_required(VERSION 3.14)
project(freertos_cpp_wrappers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add option for coverage
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Coverage flags
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Find GoogleTest - try FetchContent if PkgConfig fails
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(GTEST gtest_main)
    pkg_check_modules(GMOCK gmock_main)
endif()

# Only build tests if GoogleTest is available
if(GTEST_FOUND)
    message(STATUS "GoogleTest found via PkgConfig - building tests")
    # Add tests subdirectory
    add_subdirectory(tests)
else()
    message(WARNING "GoogleTest not found - skipping tests. Install libgtest-dev and libgmock-dev to enable testing.")
endif()

# Enable testing
enable_testing()

# Include directories
include_directories(include)
include_directories(tests/mocks)

# Add the library source files
add_library(freertos_cpp_wrappers 
    src/freertos_task.cc
)
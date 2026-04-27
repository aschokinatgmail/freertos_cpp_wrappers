cmake_minimum_required(VERSION 3.14)
project(freertos_cpp_wrappers
    VERSION 3.2.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warning flags (host builds only - cross-compile sets its own)
if(NOT CMAKE_CROSSCOMPILING)
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wold-style-cast -Wnull-dereference
    -Wdouble-promotion -Wformat=2
    -Wundef -Werror=return-type
)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/freertos_version.hpp
    @ONLY
)

# =============================================================================
# KCONFIG-BASED CONFIGURATION
# =============================================================================

# Phase 3 -- Standard Library Integration Backends
option(FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND "Enable gthr-FreeRTOS backend for GCC/libstdc++" OFF)
option(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING "Enable __external_threading backend for libc++" OFF)

# Atomic Wait/Notify
option(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY "Enable atomic wait/notify platform hooks" OFF)
set(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL "1" CACHE STRING "Atomic wait impl: 1=semaphore table, 2=task notifications, 3=spin-only")
set(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE "16" CACHE STRING "Hash table size for atomic wait semaphore table")

# Allocation Strategy
set(FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY "3" CACHE STRING "Allocation strategy: 1=dynamic, 2=static, 3=auto")

# Condition Variable Implementation
set(FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL "1" CACHE STRING "Condition variable impl: 1=counting semaphore, 2=task notifications, 3=binary semaphore per waiter")

# C++ Standard Library Features
option(FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD "Enable <thread> support" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX "Enable <mutex>/<condition_variable>/<call_once> support" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_STD_SHARED_MUTEX "Enable <shared_mutex> support" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_STD_FUTURE "Enable <future>/<promise>/<async> support" OFF)

# Library Feature Toggles
option(FREERTOS_CPP_WRAPPERS_ENABLE_EXPECTED "Enable freertos::expected" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_SPAN "Enable freertos::span" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_THREAD_SAFETY "Enable thread safety annotations" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_CONCEPTS "Enable C++20 concepts" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_ALLOCATOR "Enable external allocator support" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_ISR_RESULT "Enable ISR result types" ON)
option(FREERTOS_CPP_WRAPPERS_ENABLE_STRONG_TYPES "Enable strong type wrappers" ON)

# Convert boolean CMake options to 0/1 for C++ macros
foreach(_opt
    ENABLE_GTHR_BACKEND
    ENABLE_EXTERNAL_THREADING
    ENABLE_ATOMIC_WAIT_NOTIFY
    ENABLE_STD_THREAD
    ENABLE_STD_MUTEX
    ENABLE_STD_SHARED_MUTEX
    ENABLE_STD_FUTURE
    ENABLE_EXPECTED
    ENABLE_SPAN
    ENABLE_THREAD_SAFETY
    ENABLE_CONCEPTS
    ENABLE_EXTERNAL_ALLOCATOR
    ENABLE_ISR_RESULT
    ENABLE_STRONG_TYPES
)
    if(FREERTOS_CPP_WRAPPERS_${_opt})
        set(FREERTOS_CPP_WRAPPERS_${_opt} 1)
    else()
        set(FREERTOS_CPP_WRAPPERS_${_opt} 0)
    endif()
endforeach()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_cpp_wrappers_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/freertos_cpp_wrappers_config.hpp
    @ONLY
)

# =============================================================================
# HOST-BUILD ONLY: library, tests, static analysis, docs, install
# =============================================================================
if(NOT CMAKE_CROSSCOMPILING)

# Add option for coverage
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Coverage flags
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# =============================================================================
# STATIC ANALYSIS CONFIGURATION (clang-tidy)
# =============================================================================

# Option to enable/disable clang-tidy static analysis for library modules
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis for library modules" ON)

# Option to treat clang-tidy warnings as errors (useful for CI)
option(CLANG_TIDY_WARNINGS_AS_ERRORS "Treat clang-tidy warnings as errors" OFF)

# Option to generate clang-tidy report
option(GENERATE_CLANG_TIDY_REPORT "Generate clang-tidy static analysis report" ON)

# Find GoogleTest - try FetchContent if PkgConfig fails
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(GTEST gtest_main)
    pkg_check_modules(GMOCK gmock_main)
endif()

# Enable testing
enable_testing()

# Include directories
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
# Note: mocks directory will be added only to test targets to avoid clang-tidy issues

# Add the library source files - include all modules for static analysis
add_library(freertos_cpp_wrappers 
    src/freertos_task.cc
    include/freertos_event_group.hpp
    include/freertos_message_buffer.hpp
    include/freertos_queue.hpp
    include/freertos_semaphore.hpp
    include/freertos_stream_buffer.hpp
    include/freertos_sw_timer.hpp
    include/freertos_task.hpp
    include/freertos.hpp
)

if(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
    target_sources(freertos_cpp_wrappers PRIVATE src/freertos_atomic_wait.cc)
    target_compile_definitions(freertos_cpp_wrappers PUBLIC
        FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=1
        FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=${FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL}
        FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE=${FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE}
    )
endif()

# Add include directories to the library target
target_include_directories(freertos_cpp_wrappers PRIVATE tests/mocks)

target_compile_features(freertos_cpp_wrappers PUBLIC cxx_std_17)

# For coverage builds, disable exceptions for library code to reduce GCC artifact branches
if(ENABLE_COVERAGE)
    target_compile_options(freertos_cpp_wrappers PRIVATE -fno-exceptions)
endif()

# =============================================================================
# CODE FORMATTING CONFIGURATION (clang-format)
# =============================================================================

# Find clang-format executable
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    
    # Add custom target for code formatting
    add_custom_target(format
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/format_code.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting C++ source files according to coding standard"
    )
    
    message(STATUS "Run 'make format' to format all C++ source files")
else()
    message(WARNING "clang-format not found! Code formatting will be skipped. Install clang-format to enable code formatting.")
endif()

# =============================================================================
# APPLY STATIC ANALYSIS TO LIBRARY MODULES ONLY
# =============================================================================

if(ENABLE_CLANG_TIDY)
    # Find clang-tidy executable
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        
        # Define clang-tidy checks for library modules
        # To extend or modify the check set, update the list below:
        # - cppcoreguidelines-*: C++ Core Guidelines compliance
        # - cert-*: CERT C++ Secure Coding Standard
        # - google-*: Google C++ Style Guide compliance
        # - hicpp-*: High Integrity C++ Coding Standard
        # - bugprone-*: Common programming errors (misused casts, sizeof, dangling pointers)
        # - performance-*: Unnecessary copies, move semantic issues
        # - concurrency-*: Thread safety and concurrency issues
        # - modernize-*: C++17 modernization recommendations
        # - readability-*: Naming, magic numbers, function complexity
        # Exclusions are required for FreeRTOS C API integration patterns:
        # - bugprone-easily-swappable-parameters: FreeRTOS uses similar-typed params (TickType_t, UBaseType_t)
        # - bugprone-narrowing-conversions: FreeRTOS uses mixed integer types
        # - cppcoreguidelines-pro-type-reinterpret-cast: Required for void* callback pattern
        # - cppcoreguidelines-pro-type-vararg: configASSERT and FreeRTOS API patterns
        # - cppcoreguidelines-avoid-magic-numbers: FreeRTOS constants (pdTRUE, pdPASS)
        # - google-readability-todo: TODO format is project-specific
        # - modernize-use-trailing-return-type: Style preference
        # - readability-function-cognitive-complexity: Wrapper functions are necessarily complex
        # - readability-identifier-length: FreeRTOS naming uses short names
        # - readability-magic-numbers: FreeRTOS constants are opaque integers
        set(CLANG_TIDY_CHECKS
            "cppcoreguidelines-*,cert-*,google-*,hicpp-*,bugprone-*,concurrency-*,modernize-*,performance-*,readability-*,-bugprone-easily-swappable-parameters,-bugprone-narrowing-conversions,-cppcoreguidelines-pro-type-reinterpret-cast,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-avoid-magic-numbers,-google-readability-todo,-modernize-use-trailing-return-type,-readability-function-cognitive-complexity,-readability-identifier-length,-readability-magic-numbers"
        )
        
        # Set up clang-tidy command with report generation
        if(GENERATE_CLANG_TIDY_REPORT)
            set(CLANG_TIDY_REPORT_FILE "${CMAKE_SOURCE_DIR}/VnV/STATIC_ANALYSIS_REPORT.md")
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/VnV")
            message(STATUS "clang-tidy report will be generated at: ${CLANG_TIDY_REPORT_FILE}")
        endif()
        
        # Configure clang-tidy for library targets only
        # Note: Warnings-as-errors can be disabled during development by setting CLANG_TIDY_WARNINGS_AS_ERRORS=OFF
        if(CLANG_TIDY_WARNINGS_AS_ERRORS)
            if(GENERATE_CLANG_TIDY_REPORT)
                set(CLANG_TIDY_COMMAND 
                    ${CLANG_TIDY_EXE}
                    --checks=${CLANG_TIDY_CHECKS}
                    --warnings-as-errors=*
                    --export-fixes=${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml
                )
            else()
                set(CLANG_TIDY_COMMAND 
                    ${CLANG_TIDY_EXE}
                    --checks=${CLANG_TIDY_CHECKS}
                    --warnings-as-errors=*
                    --quiet
                )
            endif()
            message(STATUS "clang-tidy warnings will be treated as errors for library modules")
        else()
            if(GENERATE_CLANG_TIDY_REPORT)
                set(CLANG_TIDY_COMMAND 
                    ${CLANG_TIDY_EXE}
                    --checks=${CLANG_TIDY_CHECKS}
                    --export-fixes=${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml
                )
            else()
                set(CLANG_TIDY_COMMAND 
                    ${CLANG_TIDY_EXE}
                    --checks=${CLANG_TIDY_CHECKS}
                    --quiet
                )
            endif()
            message(STATUS "clang-tidy warnings will NOT be treated as errors for library modules")
        endif()
        
        # Apply clang-tidy only to the library target
        set_target_properties(freertos_cpp_wrappers PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
        )
        
        # Create a custom target for generating a detailed static analysis report with statistics
        if(GENERATE_CLANG_TIDY_REPORT)
            add_custom_target(static-analysis-report
                # Run clang-tidy and capture output for all library modules (both .cc and .hpp files)
                COMMAND ${CLANG_TIDY_EXE} 
                    --checks=${CLANG_TIDY_CHECKS}
                    --format-style=file
                    --export-fixes=${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml
                    --system-headers=false
                    --header-filter="^${CMAKE_CURRENT_SOURCE_DIR}/include/.*"
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_event_group.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_message_buffer.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_queue.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_semaphore.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_stream_buffer.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_sw_timer.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_task.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/freertos_atomic_wait.hpp
                    -- 
                    -I${CMAKE_CURRENT_SOURCE_DIR}/include
                    -isystem${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
                    -std=c++17
                    -DUNIT_TESTING=1
                    > ${CMAKE_BINARY_DIR}/clang-tidy-report.txt 2>&1 || true
                
                # Generate combined static analysis report (clang-tidy + MISRA + Enhanced cppcheck)
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_misra_analysis.sh
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}/misra_analysis_output.txt
                
                # Generate enhanced cppcheck analysis with all rules
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_enhanced_cppcheck_analysis.sh
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}/enhanced_cppcheck_output.txt
                
                # Generate integrated report using shell script
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_static_analysis_report.sh
                    ${CMAKE_BINARY_DIR}/clang-tidy-report.txt
                    ${CMAKE_BINARY_DIR}/clang-tidy-stats.md
                    ${CLANG_TIDY_REPORT_FILE}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}/misra_analysis_output.txt
                    ${CMAKE_BINARY_DIR}/enhanced_cppcheck_output.txt
                
                # Status messages
                COMMAND ${CMAKE_COMMAND} -E echo "Fixes exported to: ${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml"
                
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating comprehensive static analysis report with statistics for library modules (clang-tidy + MISRA + enhanced cppcheck)"
            )
            message(STATUS "Run 'make static-analysis-report' to generate a detailed static analysis report")
        endif()
        
        message(STATUS "clang-tidy enabled for library modules with checks: ${CLANG_TIDY_CHECKS}")
    else()
        message(WARNING "clang-tidy not found! Static analysis will be skipped. Install clang-tidy to enable static analysis.")
    endif()
else()
    message(STATUS "clang-tidy disabled via ENABLE_CLANG_TIDY=OFF")
endif()

# =============================================================================
# MISRA C++ STATIC ANALYSIS CONFIGURATION (cppcheck)
# =============================================================================

# Option to enable/disable MISRA C++ static analysis
option(ENABLE_MISRA_ANALYSIS "Enable MISRA C++ static analysis using cppcheck" ON)

if(ENABLE_MISRA_ANALYSIS)
    # Find cppcheck executable
    find_program(CPPCHECK_EXE NAMES cppcheck)
    
    if(CPPCHECK_EXE)
        message(STATUS "cppcheck found: ${CPPCHECK_EXE}")
        message(STATUS "MISRA C++ analysis will be integrated into static analysis report")
        message(STATUS "Enhanced cppcheck analysis enabled - includes all rules (style, performance, portability, security, etc.)")
        message(STATUS "MISRA C++ analysis enabled - uses MISRA C 2012 rules applicable to C++")
    else()
        message(WARNING "cppcheck not found! MISRA C++ analysis will be skipped. Install cppcheck to enable MISRA analysis.")
    endif()
else()
    message(STATUS "MISRA C++ analysis disabled via ENABLE_MISRA_ANALYSIS=OFF")
endif()

# =============================================================================
# END MISRA C++ STATIC ANALYSIS CONFIGURATION
# =============================================================================

# =============================================================================
# VALIDATION AND VERIFICATION REPORT GENERATION
# =============================================================================

# Add custom target for generating combined validation and verification report
# Output: VnV/VALIDATION_VERIFICATION_REPORT_<YYYYMMDD_HHMMSS>.md/.html
add_custom_target(validation-verification-report
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_validation_verification_report.sh
        ${CMAKE_BINARY_DIR}
    
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating combined validation and verification report in VnV/ directory"
)

# Make the reports depend on having coverage enabled
if(NOT ENABLE_COVERAGE)
    message(WARNING "Coverage not enabled. Run with -DENABLE_COVERAGE=ON for complete validation reports.")
endif()

message(STATUS "Run 'make validation-verification-report' to generate combined V&V report in VnV/ directory")

# =============================================================================
# DOCUMENTATION GENERATION
# =============================================================================

# Add option for documentation generation
option(GENERATE_DOCS "Generate documentation" ON)

if(GENERATE_DOCS)
    # Find doxygen executable
    find_program(DOXYGEN_EXE NAMES doxygen)
    
    if(DOXYGEN_EXE)
        message(STATUS "doxygen found: ${DOXYGEN_EXE}")
        
        # Add custom target for generating documentation
        add_custom_target(docs
            # Generate documentation with dynamic version
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_documentation.sh
            
            # Status messages
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "=== Documentation Generated ==="
            COMMAND ${CMAKE_COMMAND} -E echo "Documentation available at: ${CMAKE_CURRENT_SOURCE_DIR}/docs/html/index.html"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating project documentation with dynamic version"
        )
        
        message(STATUS "Run 'make docs' to generate documentation")
    else()
        message(WARNING "doxygen not found! Documentation generation will be skipped. Install doxygen to enable documentation generation.")
    endif()
else()
    message(STATUS "Documentation generation disabled via GENERATE_DOCS=OFF")
endif()

# =============================================================================
# END DOCUMENTATION GENERATION
# =============================================================================

# =============================================================================
# END VALIDATION AND VERIFICATION REPORT GENERATION
# =============================================================================

# Only build tests if GoogleTest is available
if(GTEST_FOUND OR GTest_FOUND)
    message(STATUS "GoogleTest found via PkgConfig - building tests")
    # Add tests subdirectory
    add_subdirectory(tests)
else()
    message(WARNING "GoogleTest not found - skipping tests. Install libgtest-dev and libgmock-dev to enable testing.")
endif()

# Simulation tests with real FreeRTOS kernel
option(ENABLE_SIMULATION "Enable FreeRTOS+POSIX simulation system tests" OFF)

if(ENABLE_SIMULATION)
    message(STATUS "Simulation tests enabled - building with FreeRTOS+POSIX kernel")
    add_subdirectory(tests/simulation)
endif()

endif() # NOT CMAKE_CROSSCOMPILING

# =============================================================================
# INSTALL RULES (host builds only - cross-compile installs are project-specific)
# =============================================================================
if(NOT CMAKE_CROSSCOMPILING)
include(GNUInstallDirs)

install(TARGETS freertos_cpp_wrappers
    EXPORT freertos_cpp_wrappers-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/freertos_cpp_wrappersConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/freertos_cpp_wrappersConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freertos_cpp_wrappers
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/freertos_cpp_wrappersConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freertos_cpp_wrappers
)

install(EXPORT freertos_cpp_wrappers-targets
    NAMESPACE freertos::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freertos_cpp_wrappers
)
endif() # NOT CMAKE_CROSSCOMPILING

# =============================================================================
# CROSS-COMPILATION: ARM Cortex-M verification tests
# =============================================================================
option(ENABLE_CROSS_COMPILE "Enable ARM cross-compile verification test" OFF)

if(ENABLE_CROSS_COMPILE)
    message(STATUS "Cross-compile verification enabled - building for ARM Cortex-M")
    add_subdirectory(tests/cross_compile)
endif()

option(ENABLE_QEMU_TEST "Enable QEMU ARM runtime test" OFF)

if(ENABLE_QEMU_TEST)
    message(STATUS "QEMU ARM runtime test enabled - building for ARM Cortex-M + QEMU")
    add_subdirectory(tests/qemu)
endif()

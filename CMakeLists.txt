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

# Add include directories to the library target
target_include_directories(freertos_cpp_wrappers PRIVATE tests/mocks)

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
        # Add additional check patterns with commas, or exclude specific checks with '-pattern'
        # For faster builds during development, consider using a subset like: "readability-*,performance-*"
        set(CLANG_TIDY_CHECKS
            "cppcoreguidelines-*,cert-*,google-*,hicpp-*"
        )
        
        # Set up clang-tidy command with report generation
        if(GENERATE_CLANG_TIDY_REPORT)
            set(CLANG_TIDY_REPORT_FILE "${CMAKE_SOURCE_DIR}/STATIC_ANALYSIS_REPORT.md")
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
                    -- 
                    -I${CMAKE_CURRENT_SOURCE_DIR}/include
                    -isystem${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
                    -std=c++17
                    -DUNIT_TESTING=1
                    > ${CMAKE_BINARY_DIR}/clang-tidy-report.txt 2>&1 || true
                
                # Generate report using shell script
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_static_analysis_report.sh
                    ${CMAKE_BINARY_DIR}/clang-tidy-report.txt
                    ${CMAKE_BINARY_DIR}/clang-tidy-stats.md
                    ${CLANG_TIDY_REPORT_FILE}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                
                # Status messages
                COMMAND ${CMAKE_COMMAND} -E echo "Fixes exported to: ${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml"
                
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating comprehensive static analysis report with statistics for library modules"
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
# VALIDATION AND VERIFICATION REPORT GENERATION
# =============================================================================

# Add custom target for generating validation and verification report
add_custom_target(validation-verification-report
    # Clean previous coverage data
    COMMAND find . -name "*.gcda" -delete || true
    
    # Build project (ensure latest code is compiled)
    COMMAND ${CMAKE_COMMAND} --build . --target freertos_cpp_wrappers
    COMMAND ${CMAKE_COMMAND} --build . --target freertos_mocks
    COMMAND ${CMAKE_COMMAND} --build . --parallel
    
    # Run all tests
    COMMAND ctest --verbose --output-on-failure
    
    # Generate coverage data
    COMMAND lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,negative,gcov
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
    
    # Generate static analysis report
    COMMAND ${CMAKE_COMMAND} --build . --target static-analysis-report
    
    # Generate documentation (after successful tests)
    COMMAND ${CMAKE_COMMAND} --build . --target docs
    
    # Generate validation and verification report
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_validation_verification_report.py 
        ${CMAKE_BINARY_DIR} 
        ${CMAKE_CURRENT_SOURCE_DIR}/VALIDATION_VERIFICATION_REPORT.md
    
    # Status messages
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=== Reports Generated ==="
    COMMAND ${CMAKE_COMMAND} -E echo "1. STATIC_ANALYSIS_REPORT.md - Static code analysis for main modules"
    COMMAND ${CMAKE_COMMAND} -E echo "2. VALIDATION_VERIFICATION_REPORT.md - Test execution and coverage analysis"
    COMMAND ${CMAKE_COMMAND} -E echo "3. docs/html/index.html - Updated project documentation"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests, generating reports, and updating documentation"
)

# Make the reports depend on having coverage enabled
if(NOT ENABLE_COVERAGE)
    message(WARNING "Coverage not enabled. Run with -DENABLE_COVERAGE=ON for complete validation reports.")
endif()

message(STATUS "Run 'make validation-verification-report' to generate both required reports automatically")

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
if(GTEST_FOUND)
    message(STATUS "GoogleTest found via PkgConfig - building tests")
    # Add tests subdirectory
    add_subdirectory(tests)
else()
    message(WARNING "GoogleTest not found - skipping tests. Install libgtest-dev and libgmock-dev to enable testing.")
endif()

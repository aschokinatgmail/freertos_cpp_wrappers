# Building the Library

This guide covers how to build the FreeRTOS C++ Wrappers library from source.

## Prerequisites

Before building, ensure you have:

### Required Tools

- **C++17 compatible compiler**
  - GCC 7.0+ 
  - Clang 6.0+
  - MSVC 2017+ (Windows)
- **CMake 3.12 or later**
- **Git** for cloning the repository

### Optional Tools

- **clang-tidy** for static analysis
- **lcov/gcov** for code coverage
- **Doxygen** for documentation generation
- **GoogleTest** for running tests

## Basic Build

### 1. Clone the Repository

```bash
git clone https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
cd freertos_cpp_wrappers
```

### 2. Create Build Directory

```bash
mkdir build && cd build
```

### 3. Configure with CMake

=== "Debug Build"

    ```bash
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    ```

=== "Release Build"

    ```bash
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```

=== "Release with Debug Info"

    ```bash
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    ```

### 4. Build the Library

```bash
make -j$(nproc)
```

Or on Windows:
```cmd
cmake --build . --config Release --parallel
```

## Advanced Build Options

### Code Coverage

To enable code coverage collection:

```bash
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

Generate coverage reports:
```bash
make validation-verification-report
```

### Static Analysis

Enable clang-tidy static analysis:

```bash
cmake -DENABLE_CLANG_TIDY=ON -DCLANG_TIDY_WARNINGS_AS_ERRORS=OFF ..
make -j$(nproc)
```

Generate static analysis report:
```bash
make static-analysis-report
```

### MISRA C++ Analysis

Enable MISRA C++ compliance checking:

```bash
cmake -DENABLE_MISRA_ANALYSIS=ON ..
make static-analysis-report
```

### Documentation Generation

Enable documentation generation:

```bash
cmake -DGENERATE_DOCS=ON ..
make docs
```

## CMake Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Debug | Build type (Debug, Release, RelWithDebInfo) |
| `ENABLE_COVERAGE` | OFF | Enable code coverage collection |
| `ENABLE_CLANG_TIDY` | ON | Enable clang-tidy static analysis |
| `CLANG_TIDY_WARNINGS_AS_ERRORS` | OFF | Treat clang-tidy warnings as errors |
| `GENERATE_CLANG_TIDY_REPORT` | ON | Generate static analysis report |
| `ENABLE_MISRA_ANALYSIS` | ON | Enable MISRA C++ analysis |
| `GENERATE_DOCS` | ON | Enable documentation generation |

## Running Tests

### Prerequisites for Testing

Install GoogleTest development libraries:

=== "Ubuntu/Debian"

    ```bash
    sudo apt-get install libgtest-dev libgmock-dev
    ```

=== "Fedora/RHEL"

    ```bash
    sudo dnf install gtest-devel gmock-devel
    ```

=== "macOS"

    ```bash
    brew install googletest
    ```

### Run All Tests

```bash
# From build directory
ctest --verbose --output-on-failure
```

### Run Specific Test Categories

```bash
# Run only task tests
ctest -R "TaskTest"

# Run tests matching pattern
ctest -R "Semaphore.*Timeout"

# List all available tests
ctest --show-only
```

## Installation

### System Installation

Install the library system-wide:

```bash
# From build directory
sudo make install
```

Default installation paths:
- Headers: `/usr/local/include/freertos_cpp_wrappers/`
- Library: `/usr/local/lib/`
- CMake config: `/usr/local/lib/cmake/freertos_cpp_wrappers/`

### Custom Installation Path

```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/freertos_cpp_wrappers ..
make install
```

### Package Installation

Create installable packages:

```bash
# DEB package (Ubuntu/Debian)
cpack -G DEB

# RPM package (Fedora/RHEL)
cpack -G RPM

# TGZ archive
cpack -G TGZ
```

## Using in Your Project

### With CMake

After installation, use in your CMake project:

```cmake
find_package(freertos_cpp_wrappers REQUIRED)

target_link_libraries(your_target 
    PRIVATE freertos_cpp_wrappers::freertos_cpp_wrappers
)
```

### Manual Integration

If not installing system-wide:

```cmake
# Add include directory
include_directories(path/to/freertos_cpp_wrappers/include)

# Link against FreeRTOS (you need to provide this)
target_link_libraries(your_target PRIVATE freertos)
```

## Platform-Specific Notes

### Windows (MSVC)

```cmd
# Configure
cmake -G "Visual Studio 16 2019" -A x64 ..

# Build
cmake --build . --config Release
```

### Cross-Compilation

For embedded targets:

```bash
# Example for ARM Cortex-M
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/arm-toolchain.cmake ..
make -j$(nproc)
```

Example toolchain file:
```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_CXX_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
```

## Troubleshooting

### Common Build Issues

!!! warning "Compiler Too Old"
    ```
    error: This file requires compiler and library support for C++17
    ```
    **Solution:** Upgrade to a C++17 compatible compiler (GCC 7+, Clang 6+)

!!! warning "GoogleTest Not Found"
    ```
    GoogleTest not found - skipping tests
    ```
    **Solution:** Install GoogleTest development packages or build without tests

!!! warning "clang-tidy Not Found"
    ```
    clang-tidy not found! Static analysis will be skipped
    ```
    **Solution:** Install clang-tidy or disable with `-DENABLE_CLANG_TIDY=OFF`

### Build Performance

For faster builds:

```bash
# Use more parallel jobs
make -j$(nproc)

# On systems with limited memory, reduce parallelism
make -j2

# Use Ninja generator (faster than Make)
cmake -G Ninja ..
ninja
```

### Memory Usage

Large parallel builds may consume significant memory:

```bash
# Monitor memory usage
watch -n 1 'free -h'

# Reduce parallelism if needed
make -j2
```

## CI/CD Integration

### GitHub Actions Example

```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y libgtest-dev libgmock-dev clang-tidy lcov
    
    - name: Configure
      run: |
        mkdir build && cd build
        cmake -DENABLE_COVERAGE=ON -DENABLE_CLANG_TIDY=ON ..
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Test
      run: |
        cd build
        ctest --verbose --output-on-failure
    
    - name: Generate Reports
      run: |
        cd build
        make validation-verification-report
```

## Next Steps

Once you have successfully built the library:

1. **[Run the Quick Start Guide](../getting-started/quick-start.md)** - Create your first program
2. **[Read the Examples](../examples/basic-usage.md)** - Learn usage patterns
3. **[Check API Reference](../api/overview.md)** - Browse the complete API
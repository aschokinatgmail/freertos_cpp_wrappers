# Contributing to FreeRTOS C++ Wrappers

## Pull Request Process

1. **Fork** the repository and create a branch from `main`
2. **Branch naming**: `feature/short-description`, `bugfix/short-description`, `safety/short-description`, or `docs/short-description`
3. **Commit messages** follow [Conventional Commits](https://www.conventionalcommits.org/):
   - `fix:` - bug fixes
   - `feat:` - new features
   - `refactor:` - code restructuring without behavioral change
   - `docs:` - documentation changes
   - `chore:` - build/tooling changes
   - Include issue reference in parentheses: `fix(#123): description`
4. **Testing requirements**:
   - All existing tests must pass
   - New features require unit tests
   - Bug fixes should include a regression test
   - Coverage must not decrease below 95% branch coverage
5. **Static analysis**: Changes must be clean under clang-tidy and MISRA C 2012 analysis
6. **Code review**: At least one approving review required for merge
7. **Safety-critical changes**: Changes with `safety-impact:high` or `safety-impact:medium` labels require a second reviewer with domain knowledge

## Development Setup

```bash
# Clone
git clone https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
cd freertos_cpp_wrappers

# Build and test (Docker)
docker compose build test
docker compose run test

# Build and test (local)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure

# Coverage
cmake -B build-cov -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build-cov -j$(nproc)
ctest --test-dir build-cov
gcovr --filter include/ --filter src/ --print-summary build-cov
```

## Code Style

- C++17 standard
- LLVM-based formatting via `.clang-format`
- MISRA C 2012 compliance for embedded safety
- Doxygen documentation for public API
- RAII resource management, no raw `new`/`delete` in public API
- `freertos::expected<T,E>` for error handling (no exceptions in embedded paths)

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

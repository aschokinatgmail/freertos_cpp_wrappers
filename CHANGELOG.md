# Changelog

All notable changes to the FreeRTOS C++ Wrappers Library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.1.0] - Unreleased

### Added
- CMake install() rules and C++17 propagation to consumers (#125)
- `[[nodiscard]]` on query methods across all wrappers (#131)
- `task::notify_wait_ex()`, `notify_ex()`, `notify_give_ex()` returning `expected<T, error>` (#132)
- `expected<T,E>` polyfill: `operator==`/`operator!=`, `value_or()`, `const &&` overloads (#126)
- FreeRTOS config preset files: minimal, standard, full, safety, simulation (#129)
- CHANGELOG.md (#133)

### Changed
- `task::resume_isr()` now returns `isr_result<BaseType_t>` for consistency with ISR patterns (#123)
- `const` qualifiers added to query methods that don't modify state (#124)

### Fixed
- `event_group::set_bits_ex_isr()` now properly returns error on failure instead of success (#122)
- README coverage badge and test count updated to match V&V report (#127)
- Doxygen-generated files removed from Git tracking (#128)
- Version inconsistencies in header files corrected (#133)

## [3.0.1] - 2026-04-20

### Fixed
- Move/swap operations now properly transfer allocators, fixing use-after-free with static allocators (#119)
- Timer move assignment restructured with proper rollback on recreation failure (#120, #136)
- `m_started` made volatile to prevent compiler reordering between ISR and task contexts (#121)
- Static allocator move constructors enabled from `= delete` to `= default` (#137)
- Bit-field swap patterns standardized to ternary `? 1 : 0` (#138)
- 27 new regression tests for dynamic and static allocator move/swap paths

## [3.0.0] - 2026-04-19

### Added
- Complete FreeRTOS C++ wrapper library with RAII semantics
- Static and dynamic allocation support
- `expected<T,E>` error handling with `*_ex()` method variants
- External allocator support for pre-existing kernel objects
- `std::chrono` timeout support
- Thread safety annotations
- ISR-safe operation variants
- Move semantics for all wrapper types
- Doxygen documentation
- 592 unit tests with 95% branch coverage

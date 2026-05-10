# Changelog

All notable changes to the FreeRTOS C++ Wrappers Library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.2.1] - Unreleased

### Fixed
- Buffer reset_isr signatures now compile against pre-V10.6 FreeRTOS via `tskKERNEL_VERSION` guard (#302)
- `to_ticks_saturating()` rewritten in 64-bit math; closes silent-corruption window (#303)
- `is_contiguous_iterator_v` no longer false-positives on `std::deque::iterator` (#306)
- `expected::value()` / `error()` / `operator->` accessors hardened with `FREERTOS_EXPECTED_REQUIRE` macro (#337)
- `xEventGroupClearBitsFromISR` mock null-guards `g_freertos_mock` (#316)
- `sw_timer` ISR `_ex` variants distinguish `error::invalid_handle` from `error::timer_queue_full` (#304)
- `event_group::set_bits_ex_isr` returns correct error code for queue-full failures (#305)
- `buffer_full` / `buffer_empty` / `message_too_large` error codes wired into `_ex` paths (#307)
- `mutex::swap()` refuses to operate while RAII guards reference the mutex (#338)
- `condition_variable_any` destructor asserts no pending waiters (#250)
- External allocators assert against double-create (#253)
- `atomic_flag notify_all` guards against count underflow (#255)
- Counting semaphore allocators forward `initial_count` parameter (#229)
- Static queue move/swap guarded by `configASSERT` (#258)
- `event_group::wait_bits_ex` requires all bits when `wait_for_all_bits=true` (#265)
- `atomic_wait` table size enforced as power-of-two (#240)
- Wake-batch stack usage bounded and documented for IMPL=2 (#249)
- Version strings reconciled to 3.2.1 across project (#318)
- `.clang-format` updated for clang-format 16+ compatibility (#329)

### Documentation
- Removed stale `yield_if_needed()` references from `freertos_isr_result.hpp` doxygen (#301)
- `freertos_atomic_wait.cc` version stamp synchronized (#308)
- `event_group` ISR doxygen corrections (#310, #311)
- `delay_until` chrono overloads document sub-millisecond truncation (#314)

## [3.1.0] - Unreleased

### Added
- CMake install() rules and C++17 propagation to consumers (#125)
- `[[nodiscard]]` on query methods across all wrappers (#131)
- `task::notify_wait_ex()`, `notify_ex()`, `notify_give_ex()` returning `expected<T, error>` (#132)
- `expected<T,E>` polyfill: `operator==`/`operator!=`, `value_or()`, `const &&` overloads (#126)
- FreeRTOS config preset files: minimal, standard, full, safety, simulation (#129)
- CHANGELOG.md (#133)
- Native simulation test suites running C++ wrappers against the real
  FreeRTOS+POSIX kernel — 11 category test binaries (semaphore, mutex,
  queue, stream_buffer, message_buffer, timer, event_group, task,
  move_swap, isr_api, concurrency) (#118)
- `isr_result<void>` specialization for ISR wrappers with no return value (#118)
- `FREERTOS_CPP_WRAPPERS_SIMULATION` compile-time guard for POSIX-port API
  gaps (`is_isr()` → false, `timer_daemon_task_handle()` → configASSERT)
- `add_sim_test()` CMake helper for simulation test targets with
  configurable timeout and automatic labels (#118)
- Shared `sim_app_hooks.cpp` for FreeRTOS static allocation hooks (#144)
- Simulation test README documenting 1-test-per-binary convention and
  `EXPECT_TRUE` multi-assertion pattern (#145, #146)

### Changed
- `task::resume_isr()` now returns `isr_result<BaseType_t>` for consistency with ISR patterns (#123)
- `const` qualifiers added to query methods that don't modify state (#124)
- `freertos_task.hpp` include order adjusted for FreeRTOS+POSIX compatibility (#142)
- `SIMULATION` macro renamed to `FREERTOS_CPP_WRAPPERS_SIMULATION` to avoid namespace collisions (#140)
- `timer_daemon_task_handle()` now uses `configASSERT` under simulation instead of silently returning nullptr (#141)
- Simulation test result flags migrated from `volatile bool`/`volatile int` to `std::atomic<bool>`/`std::atomic<int>` with `memory_order_relaxed` semantics (#151)
- Duplicated FreeRTOS application-memory hooks extracted from 12 test files into shared `sim_app_hooks.cpp` (#144)
- Parameter name typo `ppxIdleTaskStackBuffer` corrected to `ppxTimerTaskStackBuffer` in `vApplicationGetTimerTaskMemory` (#139)
- `test_sim_isr.cpp` renamed description: tests exercise ISR API surface from task context only (#152)
- `add_sim_test()` now accepts optional `TIMEOUT` keyword argument (#153)
- `target_link_libraries(... PUBLIC ...)` in simulation CMakeLists.txt documented with intent comments (#154)
- `isr_result<void>` specialization documented: no `.result` member, only `.higher_priority_task_woken` (#147)
- Indentation of `isr_result<void>` comparison operators aligned with project style (#148)

### Fixed
- `event_group::set_bits_ex_isr()` now properly returns error on failure instead of success (#122)
- README coverage badge and test count updated to match V&V report (#127)
- Doxygen-generated files removed from Git tracking (#128)
- Version inconsistencies in header files corrected (#133)
- Unused `<event_groups.h>` include removed from `freertos_task.hpp` (#142)
- Redundant field reinitialization removed from simulation test orchestrators (#156)
- Missing EOF newlines added to `freertos_isr_result.hpp` and `tests/simulation/CMakeLists.txt` (#149)

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

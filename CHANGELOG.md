# Changelog

All notable changes to the FreeRTOS C++ Wrappers Library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.2.1] - Unreleased

### Fixed
- Buffer reset_isr signatures now compile against pre-V10.6 FreeRTOS via `tskKERNEL_VERSION` guard (#302) — applies to `stream_buffer`, `stream_batching_buffer`, `message_buffer`
- `to_ticks_saturating()` rewritten in 64-bit math; closes the silent-corruption window between ms ≈ 4.3M and ms ≈ 4.29B at 1 kHz tick rate (#303, supersedes #287)
- `is_contiguous_iterator_v` no longer false-positives on `std::deque::iterator` (#306, supersedes #288). C++20 path uses `std::contiguous_iterator`; C++17 path narrowed to raw pointers only to honor the heap-purity directive — container iterators correctly fall through to the chunked stack-buffer fallback.
- `expected::value()` / `error()` / `operator->` accessors no longer rely solely on `configASSERT` for safety; new `FREERTOS_EXPECTED_REQUIRE` macro pairs `configASSERT` with `__builtin_trap` so wrong-state access fails loudly even when `configASSERT` is a release-build no-op (#337)
- `xEventGroupClearBitsFromISR` mock now null-guards `g_freertos_mock` consistent with every other mock (#316)
- `sw_timer` ISR `_ex` variants distinguish `error::invalid_handle` (null handle) from `error::timer_queue_full` (queue full); applies to `start_ex_isr`, `stop_ex_isr`, `reset_ex_isr`, `period_ex_isr` (#304)
- `event_group::set_bits_ex_isr` returns `error::timer_queue_full` for queue-full failures instead of `error::invalid_handle` (#305)
- `buffer_full` / `buffer_empty` / `message_too_large` error codes now wired into `stream_buffer` / `message_buffer` / `stream_batching_buffer` `_ex` paths; `would_block` reserved for genuinely-non-blocking polling that can't yet make progress (#307)
- `mutex::swap()` and `recursive_mutex::swap()` now refuse to operate while RAII guards reference the mutex; `m_guard_refcount` atomic + friend-managed inc/dec on lock_guard / try_lock_guard / timeout_lock_guard prevents the unsafe pattern where a guard's destructor would unlock a swapped-in semaphore (#338)

### Documentation
- Removed stale `yield_if_needed()` references from `freertos_isr_result.hpp` doxygen (#301)
- `freertos_atomic_wait.cc` version stamp synchronized to current release (#308)
- `event_group::set_bits_isr` doxygen `@return` corrected to `isr_result<BaseType_t>` (#310)
- `event_group::clear_bits_isr` now has full doxygen header explaining the 2-arg API and pdFALSE flag (#311)
- `delay_until` chrono overloads document sub-millisecond truncation and recommend `tick_timer` for tick-aligned planning (#314)

### Cleanup
- Removed dead `m_semaphore_created` field from `condition_variable_any` (#309)
- Rewrote double-checked-locking pattern in `freertos_atomic_wait.cc` so the outer/inner null checks use distinct operators (`!= nullptr` vs `== nullptr`); silences cppcheck `identicalInnerCondition` without suppression (#341)
- Removed unused `FREERTOS_CPP_WRAPPERS_LATCH_MAX_WAITERS` macro (#245)

### Deferred to v3.3.0
- Library-wide null-check policy for moved-from objects and unlock-failure surfacing (#275, #312, #313, #339) — partial work in `hotfix/v3.2.1-pr-g` branch covering 3 of 8 wrappers
- `std::string` / `std::function` / heap-dependent stdlib purge from public headers (#342, #300, #331, #345) — partial work in `worktree-agent-a3191237587d84b66`

## [3.2.0] - 2026-04-27

### Added
- `to_ticks_saturating()` helper for safe long-duration tick conversion (#287; corrected in v3.2.1 #303)
- New error enumerators: `timer_queue_full`, `buffer_full`, `buffer_empty`, `message_too_large` (#295)
- `isr_result` accessor methods (`result_value()`, `should_yield()`) for non-breaking encapsulation (#298)
- Documented atomic_wait Implementation 1 hash-collision limitation with quantitative collision math (#279)
- `__cpp_exceptions` guard in `once_flag` for `-fno-exceptions` toolchains

### Fixed
- `xEventGroupClearBitsFromISR` wrapper signature corrected (3-arg → 2-arg per production FreeRTOS) (#298 sim build)
- `xMessageBufferResetFromISR` / `xStreamBufferResetFromISR` wrapper signatures corrected (2-arg → 1-arg per V10.6+)
- `xStreamBufferGenericCreate` now uses 5-arg signature with version guard for V10.6+
- `condition_variable_any::wait` overflow now prevented via CAS-based admission control (#280)
- `atomic_wait` Implementation 2 mutex-creation race hardened with explicit publish ordering inside critical section (#285)
- `task::delay_until(chrono::time_point)` chrono overload now performs explicit `duration_cast<milliseconds>` (#292)
- `counting_semaphore` post-increment/decrement operators removed (cannot satisfy C++ contract on non-copyable type) (#284)
- `[[nodiscard]]` consistency pass across ~30 status-returning methods (#286)
- Version stamps unified to 3.2.0 across headers and CMakeLists.txt (#290)
- `(void)` casts on internal `m_mutex.unlock()` calls in lock-guard destructors

### Documentation
- `set_bits_ex` / `clear_bits_ex` always-succeed semantics documented (#289)
- `set_bits_ex_isr` async-bits-not-actual semantics documented (#294)
- `timer::m_started` user-request-vs-actual-state semantics documented (#293)
- `atomic_wait` Implementation 1 limitation block comment with use-Impl-2 decision tree

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

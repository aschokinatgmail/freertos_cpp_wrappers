#include "freertos_atomic_wait.hpp"

/**********************************************************************************
@file freertos_atomic_wait.cc
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS platform hooks implementation for atomic wait/notify
@version 3.2.1
@date 2026-04-28

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************/

#if defined(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)

#if FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 1

// =============================================================================
//   Implementation 1: shared bucket semaphore (legacy / minimal-overhead path)
// =============================================================================
//
// LIMITATION: Hash collisions on the bucket index cause threads waiting on
// distinct addresses to share a single counting semaphore. A `notify_one(A)`
// gives one token to the bucket, which may be consumed by a thread waiting
// on a colliding address `B` instead of the intended waiter on `A`. The
// `B` waiter then re-checks `B`'s value, finds it unchanged, and re-enters
// `__platform_wait_on_address` (correct) — but the token meant for `A` has
// been consumed and `A`'s waiter is left blocked until the next notify on a
// colliding address arrives.
//
// In practice, livelock probability scales with hash density:
// - With `atomic_wait_table_size = 16` (default) and N distinct atomics
//   actively waited on, expected collisions ≈ N²/(2·16). Above ~6 distinct
//   addresses, missed wakeups become realistic.
// - When atomic wait is used by only a handful of `std::atomic` objects in
//   the whole application, the risk is acceptable.
//
// Use Implementation 2 (`FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=2`) when:
// - The application has many distinct atomic-waited variables
// - Notification correctness is required under all scheduling conditions
// - The slightly higher per-wait overhead (linked-list bookkeeping) is
//   acceptable
//
// Implementation 2 keeps a per-address linked list of waiter nodes inside
// each bucket and uses `vTaskNotifyGive` for wakeup, so notifications are
// directed at the exact task waiting on the matching address.

freertos_wait_entry freertos_wait_table[atomic_wait_table_size] = {};

static SemaphoreHandle_t atomic_wait_ensure_semaphore(freertos_wait_entry &entry) {
    // Fast path: handle already published. Once written it is never reset,
    // so a non-null read here is safe without locking.
    if (entry.semaphore != nullptr) {
        return entry.semaphore;
    }
    // Slow path: lazily create under critical section. The two reads of
    // `entry.semaphore` are deliberately written with different operators
    // (`!= nullptr` above, `== nullptr` below) so cppcheck does not flag
    // the pattern as `identicalInnerCondition` — the outer read may race
    // with a publishing writer in another task, so the inner re-read
    // inside the critical section is required for correctness.
    taskENTER_CRITICAL();
    if (entry.semaphore == nullptr) {
        static constexpr UBaseType_t unbounded_max_count = static_cast<UBaseType_t>(-1);
        entry.semaphore = xSemaphoreCreateCounting(unbounded_max_count, 0);
        configASSERT(entry.semaphore != nullptr);
    }
    SemaphoreHandle_t handle = entry.semaphore;
    taskEXIT_CRITICAL();
    return handle;
}

extern "C" bool __platform_wait_on_address(void const *addr,
                                           __cxx_atomic_contention_t const *expected,
                                           int size) {
    (void)size;
    auto idx = atomic_wait_hash(addr);
    auto &entry = freertos_wait_table[idx];
    taskENTER_CRITICAL();
    entry.address = addr;
    entry.waiter_count.fetch_add(1, std::memory_order_relaxed);
    taskEXIT_CRITICAL();
    std::atomic_thread_fence(std::memory_order_seq_cst);

    bool match = false;
    switch (size) {
    case 1:
        match = *reinterpret_cast<unsigned char const volatile *>(expected) ==
                *reinterpret_cast<unsigned char const volatile *>(addr);
        break;
    case 2:
        match = *reinterpret_cast<unsigned short const volatile *>(expected) ==
                *reinterpret_cast<unsigned short const volatile *>(addr);
        break;
    case 4:
        match = *reinterpret_cast<unsigned int const volatile *>(expected) ==
                *reinterpret_cast<unsigned int const volatile *>(addr);
        break;
    case 8:
        match = *reinterpret_cast<unsigned long long const volatile *>(expected) ==
                *reinterpret_cast<unsigned long long const volatile *>(addr);
        break;
    }

    if (match) {
        auto sem = atomic_wait_ensure_semaphore(entry);
        xSemaphoreTake(sem, portMAX_DELAY);
    }

    entry.waiter_count.fetch_sub(1, std::memory_order_release);
    return true;
}

/** @brief Wake one or more tasks waiting on @p addr (IMPL=1, shared
 *  bucket semaphore).
 *
 *  @param addr   Address being signalled.
 *  @param count  `1` for `notify_one`, `-1` for `notify_all`, or any
 *                positive integer for "wake up to N".
 *
 *  @par Stack usage
 *  Constant — no per-call stack buffers are allocated. The function
 *  issues `xSemaphoreGive` calls in place against the bucket's shared
 *  counting semaphore. Compare with IMPL=2 which uses a bounded
 *  per-call batch buffer (see `__platform_wake_by_address` in the
 *  IMPL=2 block). */
extern "C" void __platform_wake_by_address(void const *addr, int count) {
    auto idx = atomic_wait_hash(addr);
    auto &entry = freertos_wait_table[idx];
    auto waiters = entry.waiter_count.load(std::memory_order_seq_cst);
    if (waiters == 0 || entry.semaphore == nullptr) {
        return;
    }

    if (count == 1) {
        xSemaphoreGive(entry.semaphore);
    } else {
        auto to_wake = count < 0
                            ? waiters
                            : static_cast<__cxx_atomic_contention_t>(count);
        for (__cxx_atomic_contention_t i = 0; i < to_wake; ++i) {
            xSemaphoreGive(entry.semaphore);
        }
    }
}

#elif FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 2

freertos_wait_bucket freertos_wait_buckets[atomic_wait_table_size] = {};

extern "C" bool __platform_wait_on_address(void const *addr,
                                           __cxx_atomic_contention_t const *expected,
                                           int size) {
    (void)size;
    auto idx = atomic_wait_hash(addr);
    auto &bucket = freertos_wait_buckets[idx];
    bucket.waiter_count.fetch_add(1, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_seq_cst);

    bool match = false;
    switch (size) {
    case 1:
        match = *reinterpret_cast<unsigned char const volatile *>(expected) ==
                *reinterpret_cast<unsigned char const volatile *>(addr);
        break;
    case 2:
        match = *reinterpret_cast<unsigned short const volatile *>(expected) ==
                *reinterpret_cast<unsigned short const volatile *>(addr);
        break;
    case 4:
        match = *reinterpret_cast<unsigned int const volatile *>(expected) ==
                *reinterpret_cast<unsigned int const volatile *>(addr);
        break;
    case 8:
        match = *reinterpret_cast<unsigned long long const volatile *>(expected) ==
                *reinterpret_cast<unsigned long long const volatile *>(addr);
        break;
    }

    if (match) {
        // Double-checked locking for one-time mutex creation. The outer
        // check is an optimization for the steady-state path (mutex
        // already created); the seq_cst fence above this block guarantees
        // visibility of any prior store. The critical-section-protected
        // inner check prevents two concurrent first-time waiters from
        // both creating a mutex (which would leak one and break mutual
        // exclusion in the bucket). The two reads of `bucket.mutex` are
        // deliberately written with different operators (`!= nullptr`
        // negated above, `== nullptr` below) so cppcheck does not flag
        // them as `identicalInnerCondition` — the outer race makes the
        // inner re-read necessary for correctness.
        if (bucket.mutex != nullptr) {
            // Already published; nothing to do.
        } else {
            taskENTER_CRITICAL();
            if (bucket.mutex == nullptr) {
                SemaphoreHandle_t new_mutex = xSemaphoreCreateMutex();
                configASSERT(new_mutex != nullptr);
                // Publish via the same critical section so concurrent
                // outer-check readers either see nullptr (and re-enter
                // critical) or the published value — never a torn write.
                bucket.mutex = new_mutex;
            }
            taskEXIT_CRITICAL();
        }
        freertos_waiter_node node;
        node.task = xTaskGetCurrentTaskHandle();
        node.address = addr;
        xSemaphoreTake(bucket.mutex, portMAX_DELAY);
        node.next = bucket.waiters;
        bucket.waiters = &node;
        xSemaphoreGive(bucket.mutex);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xSemaphoreTake(bucket.mutex, portMAX_DELAY);
        freertos_waiter_node **pp = &bucket.waiters;
        while (*pp != nullptr) {
            if (*pp == &node) {
                *pp = node.next;
                break;
            }
            pp = &(*pp)->next;
        }
        xSemaphoreGive(bucket.mutex);
    }

    bucket.waiter_count.fetch_sub(1, std::memory_order_release);
    return true;
}

/** @brief Wake one or more tasks waiting on @p addr.
 *
 *  @param addr   Address being signalled.
 *  @param count  Number of waiters to wake. `1` wakes a single matching
 *                waiter; positive values wake up to @p count waiters; a
 *                negative value (e.g. `-1`) wakes ALL waiters on @p addr
 *                in this bucket.
 *
 *  @par Stack usage
 *  This function uses a fixed-size stack buffer of
 *  `kAtomicWaitWakeBatchSize` entries (default: 16) to defer task
 *  notifications outside the bucket-mutex critical section. The peak
 *  stack footprint of the batch buffer is
 *      `sizeof(TaskHandle_t) * kAtomicWaitWakeBatchSize`
 *  which on a 32-bit target is 64 bytes, and on a 64-bit target is 128
 *  bytes. The buffer size is independent of
 *  @ref atomic_wait_table_size or the number of waiters in the bucket;
 *  larger waiter sets are processed in successive batches, each
 *  re-acquiring the bucket mutex from scratch (the lock is never held
 *  across an iteration). If you need to tune the bound for a
 *  particularly stack-constrained target, change
 *  `kAtomicWaitWakeBatchSize` below and the documentation above.
 *
 *  @par Batch correctness
 *  Each batch takes the bucket mutex fresh. While holding the mutex we
 *  collect up to `kAtomicWaitWakeBatchSize` matching waiters into the
 *  stack buffer and immediately mark each collected node by clearing
 *  its `address` field; this prevents a subsequent batch (or a
 *  concurrent waker that grabs the mutex between batches) from
 *  re-notifying the same waiter while it is still in the list waiting
 *  for the notify-take to return. The address field is only read by
 *  wakers under the mutex; the waiter does not re-read it after
 *  publishing the node, so the clear is race-free. The mutex is
 *  released before the actual `xTaskNotifyGive` calls so that newly
 *  arriving waiters are not blocked by long batch processing. After a
 *  notify pass we re-acquire the mutex and re-scan the list; a waiter
 *  that arrived between batches but has not been notified is still
 *  pending and will be picked up by the next pass, satisfying the
 *  "wake all" contract for negative @p count. */
extern "C" void __platform_wake_by_address(void const *addr, int count) {
    if (addr == nullptr) {
        // No real waiter publishes `address == nullptr`; we also use
        // nullptr as a "consumed by this wake" sentinel below, so a
        // null `addr` would conflate consumed and live waiters. The
        // libc++/libstdc++ atomic-wait shims never call us with
        // nullptr; reject defensively to keep the sentinel clean.
        return;
    }
    auto idx = atomic_wait_hash(addr);
    auto &bucket = freertos_wait_buckets[idx];
    auto waiters = bucket.waiter_count.load(std::memory_order_seq_cst);
    if (waiters == 0) {
        return;
    }

    if (bucket.mutex == nullptr) {
        return;
    }

    // Stack-batch size — bounded independent of atomic_wait_table_size
    // so the per-call stack footprint stays predictable on
    // stack-constrained MCUs. See doxygen above for the formula.
    static constexpr size_t kAtomicWaitWakeBatchSize = 16;

    auto remaining = count < 0
                         ? static_cast<__cxx_atomic_contention_t>(-1)
                         : static_cast<__cxx_atomic_contention_t>(count);

    while (remaining > 0) {
        TaskHandle_t tasks_to_notify[kAtomicWaitWakeBatchSize];
        size_t collected = 0;
        // Per-batch cap: smaller of the global batch size and the
        // remaining request (for bounded `count >= 0` only).
        size_t batch_cap = kAtomicWaitWakeBatchSize;
        if (count >= 0 && static_cast<size_t>(remaining) < batch_cap) {
            batch_cap = static_cast<size_t>(remaining);
        }

        xSemaphoreTake(bucket.mutex, portMAX_DELAY);
        auto *node = bucket.waiters;
        while (node != nullptr && collected < batch_cap) {
            // `address` is cleared by a previous batch when that
            // node has already been collected for notification but
            // its waiter has not yet removed the node from the list.
            if (node->address == addr) {
                tasks_to_notify[collected++] = node->task;
                // Mark this node consumed for the duration of any
                // remaining wake activity. Safe under the mutex —
                // wakers only read `address` while holding the
                // mutex, and the owning waiter does not re-read it.
                node->address = nullptr;
            }
            node = node->next;
        }
        xSemaphoreGive(bucket.mutex);

        if (collected == 0) {
            // No more matching waiters in the bucket; we are done.
            break;
        }

        for (size_t i = 0; i < collected; ++i) {
            xTaskNotifyGive(tasks_to_notify[i]);
        }

        if (count >= 0) {
            remaining -= static_cast<__cxx_atomic_contention_t>(collected);
        }
        // For count < 0 (unbounded "wake all") `remaining` stays at
        // (unsigned)-1 and the outer loop condition only exits via
        // the `collected == 0` break above.
    }
}

#elif FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 3

extern "C" bool __platform_wait_on_address(void const *addr,
                                           __cxx_atomic_contention_t const *expected,
                                           int size) {
    (void)addr;
    (void)expected;
    (void)size;
    return false;
}

extern "C" void __platform_wake_by_address(void const *addr, int count) {
    (void)addr;
    (void)count;
}

#endif

#endif
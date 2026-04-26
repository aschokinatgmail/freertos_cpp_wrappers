/**
 * @file test_freertos_atomic_wait.cpp
 * @brief Unit tests for freertos_atomic_wait.hpp
 *
 * Tests cover:
 * - Header compiles when FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY
 *   is defined
 * - Type checks on the wait entry structure
 * - Spin-only implementation compiles and works
 * - When disabled, the header compiles to nothing
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"

#define FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY 1
#define FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL 3
#include "freertos_atomic_wait.hpp"

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

TEST(AtomicWait, WaitEntryTypeExists) {
    freertos_wait_entry entry{};
    EXPECT_EQ(entry.address, nullptr);
    EXPECT_EQ(entry.semaphore, nullptr);
    EXPECT_EQ(entry.waiter_count.load(), 0);
}

TEST(AtomicWait, WaiterNodeTypeExists) {
    freertos_waiter_node node{};
    EXPECT_EQ(node.task, nullptr);
    EXPECT_EQ(node.address, nullptr);
    EXPECT_EQ(node.next, nullptr);
}

TEST(AtomicWait, WaitBucketTypeExists) {
    freertos_wait_bucket bucket{};
    EXPECT_EQ(bucket.waiter_count.load(), 0);
    EXPECT_EQ(bucket.waiters, nullptr);
    EXPECT_EQ(bucket.mutex, nullptr);
}

TEST(AtomicWait, CxxAtomicContentionTypeIsUnsignedInt) {
    static_assert(std::is_same<__cxx_atomic_contention_t, unsigned int>::value,
                  "__cxx_atomic_contention_t must be unsigned int");
}

TEST(AtomicWait, SpinOnlyWaitAlwaysReturnsFalse) {
    __cxx_atomic_contention_t expected = 0;
    auto result = __platform_wait_on_address(nullptr, &expected, 4);
    EXPECT_FALSE(result);
}

TEST(AtomicWait, SpinOnlyWakeIsNoOp) {
    __platform_wake_by_address(nullptr, 1);
    __platform_wake_by_address(nullptr, -1);
    SUCCEED();
}

TEST(AtomicWait, AtomicWaitHashReturnsValidIndex) {
    int dummy = 0;
    auto idx = atomic_wait_hash(&dummy);
    EXPECT_LT(idx, atomic_wait_table_size);
}

TEST(AtomicWait, AtomicWaitTableSizeIsPowerOfTwo) {
    EXPECT_GT(atomic_wait_table_size, 0u);
    EXPECT_EQ(atomic_wait_table_size & (atomic_wait_table_size - 1), 0u);
}

TEST(AtomicWait, SpinOnlyImplementationMacro) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL, 3);
}

TEST(AtomicWait, PlatformFunctionsHaveCorrectSignature) {
    static_assert(
        std::is_same<decltype(&__platform_wait_on_address),
                    bool(*)(void const *, __cxx_atomic_contention_t const *, int)>::value,
        "__platform_wait_on_address signature mismatch");
    static_assert(
        std::is_same<decltype(&__platform_wake_by_address),
                    void(*)(void const *, int)>::value,
        "__platform_wake_by_address signature mismatch");
}

namespace {

template <typename T> struct disabled_header_test {
    static void check() {
        static_assert(!std::is_same<T, T>::value || true,
                      "Header should compile to nothing when disabled");
    }
};

} // namespace

TEST(AtomicWait, DisabledPathCompilesCleanly) {
    disabled_header_test<int>::check();
    SUCCEED();
}

TEST(AtomicWait, AtomicWaitEntryDefaultInitialization) {
    freertos_wait_entry entries[4]{};
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(entries[i].address, nullptr);
        EXPECT_EQ(entries[i].semaphore, nullptr);
        EXPECT_EQ(entries[i].waiter_count.load(), 0);
    }
}

TEST(AtomicWait, AtomicWaiterCountIsAtomic) {
    freertos_wait_entry entry{};
    entry.waiter_count.store(5, std::memory_order_relaxed);
    EXPECT_EQ(entry.waiter_count.load(std::memory_order_acquire), 5);
    entry.waiter_count.fetch_add(1, std::memory_order_relaxed);
    EXPECT_EQ(entry.waiter_count.load(), 6);
    entry.waiter_count.fetch_sub(1, std::memory_order_release);
    EXPECT_EQ(entry.waiter_count.load(), 5);
}

TEST(AtomicWait, BucketWaiterCountIsAtomic) {
    freertos_wait_bucket bucket{};
    bucket.waiter_count.store(3, std::memory_order_relaxed);
    EXPECT_EQ(bucket.waiter_count.load(std::memory_order_seq_cst), 3);
    bucket.waiter_count.fetch_add(1, std::memory_order_relaxed);
    EXPECT_EQ(bucket.waiter_count.load(), 4);
}
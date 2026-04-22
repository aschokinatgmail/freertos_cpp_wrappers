/**********************************************************************************
@file test_freertos_lock.cpp
@brief Tests for freertos::shared_lock, freertos::unique_lock, and Lockable concepts
@version 3.1.0
@date 2026-04-22
***********************************************************************************/

#include <freertos_lock.hpp>
#include <freertos_semaphore.hpp>
#include <freertos_shared_mutex.hpp>

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>

#if FREERTOS_CPP_WRAPPERS_HAS_MUTEX && FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE

using dynamic_mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;
using dynamic_recursive_mutex = freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>;
using dynamic_shared_mutex = freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>;

TEST(SharedLockTest, DefaultConstructor) {
    freertos::shared_lock<dynamic_shared_mutex> lock;
    EXPECT_FALSE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), nullptr);
}

TEST(SharedLockTest, LockingConstructor) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock(m);
    EXPECT_TRUE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), &m);
}

TEST(SharedLockTest, DeferLock) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock(m, std::defer_lock);
    EXPECT_FALSE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), &m);
}

TEST(SharedLockTest, TryToLock) {
    dynamic_shared_mutex m;
    m.lock_shared();
    freertos::shared_lock<dynamic_shared_mutex> lock(m, std::adopt_lock);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(SharedLockTest, AdoptLock) {
    dynamic_shared_mutex m;
    m.lock_shared();
    freertos::shared_lock<dynamic_shared_mutex> lock(m, std::adopt_lock);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(SharedLockTest, MoveConstructor) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock1(m);
    freertos::shared_lock<dynamic_shared_mutex> lock2(std::move(lock1));
    EXPECT_TRUE(lock2.owns_lock());
    EXPECT_FALSE(lock1.owns_lock());
    EXPECT_EQ(lock1.mutex(), nullptr);
}

TEST(SharedLockTest, MoveAssignment) {
    dynamic_shared_mutex m1, m2;
    freertos::shared_lock<dynamic_shared_mutex> lock1(m1);
    freertos::shared_lock<dynamic_shared_mutex> lock2(m2);
    lock2 = std::move(lock1);
    EXPECT_TRUE(lock2.owns_lock());
    EXPECT_FALSE(lock1.owns_lock());
}

TEST(SharedLockTest, Release) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock(m);
    auto *released = lock.release();
    EXPECT_EQ(released, &m);
    EXPECT_FALSE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), nullptr);
    m.unlock_shared();
}

TEST(SharedLockTest, ExplicitUnlock) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock(m, std::defer_lock);
    lock.lock();
    EXPECT_TRUE(lock.owns_lock());
    lock.unlock();
    EXPECT_FALSE(lock.owns_lock());
}

TEST(SharedLockTest, Swap) {
    dynamic_shared_mutex m1, m2;
    freertos::shared_lock<dynamic_shared_mutex> lock1(m1);
    freertos::shared_lock<dynamic_shared_mutex> lock2(m2, std::defer_lock);
    lock1.swap(lock2);
    EXPECT_EQ(lock1.mutex(), &m2);
    EXPECT_FALSE(lock1.owns_lock());
    EXPECT_EQ(lock2.mutex(), &m1);
    EXPECT_TRUE(lock2.owns_lock());
}

TEST(SharedLockTest, BoolConversion) {
    dynamic_shared_mutex m;
    freertos::shared_lock<dynamic_shared_mutex> lock(m);
    EXPECT_TRUE(static_cast<bool>(lock));
    lock.unlock();
    EXPECT_FALSE(static_cast<bool>(lock));
}

TEST(UniqueLockTest, DefaultConstructor) {
    freertos::unique_lock<dynamic_mutex> lock;
    EXPECT_FALSE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), nullptr);
}

TEST(UniqueLockTest, LockingConstructor) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    EXPECT_TRUE(lock.owns_lock());
    EXPECT_EQ(lock.mutex(), &m);
}

TEST(UniqueLockTest, DeferLock) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m, std::defer_lock);
    EXPECT_FALSE(lock.owns_lock());
}

TEST(UniqueLockTest, TryToLock) {
    dynamic_mutex m;
    m.lock();
    freertos::unique_lock<dynamic_mutex> lock(m, std::adopt_lock);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(UniqueLockTest, AdoptLock) {
    dynamic_mutex m;
    m.lock();
    freertos::unique_lock<dynamic_mutex> lock(m, std::adopt_lock);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(UniqueLockTest, MoveConstructor) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock1(m);
    freertos::unique_lock<dynamic_mutex> lock2(std::move(lock1));
    EXPECT_TRUE(lock2.owns_lock());
    EXPECT_FALSE(lock1.owns_lock());
}

TEST(UniqueLockTest, MoveAssignment) {
    dynamic_mutex m1, m2;
    freertos::unique_lock<dynamic_mutex> lock1(m1);
    freertos::unique_lock<dynamic_mutex> lock2(m2);
    lock2 = std::move(lock1);
    EXPECT_TRUE(lock2.owns_lock());
}

TEST(UniqueLockTest, Release) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    auto *released = lock.release();
    EXPECT_EQ(released, &m);
    EXPECT_FALSE(lock.owns_lock());
    m.unlock();
}

TEST(UniqueLockTest, ExplicitLockUnlock) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m, std::defer_lock);
    lock.lock();
    EXPECT_TRUE(lock.owns_lock());
    lock.unlock();
    EXPECT_FALSE(lock.owns_lock());
}

TEST(UniqueLockTest, AdoptAndExplicitUnlock) {
    dynamic_mutex m;
    {
        freertos::unique_lock<dynamic_mutex> lock(m);
        EXPECT_TRUE(lock.owns_lock());
        lock.unlock();
        EXPECT_FALSE(lock.owns_lock());
    }
}

TEST(UniqueLockTest, Swap) {
    dynamic_mutex m1, m2;
    freertos::unique_lock<dynamic_mutex> lock1(m1);
    freertos::unique_lock<dynamic_mutex> lock2(m2, std::defer_lock);
    lock1.swap(lock2);
    EXPECT_EQ(lock1.mutex(), &m2);
    EXPECT_EQ(lock2.mutex(), &m1);
}

TEST(UniqueLockTest, BoolConversion) {
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    EXPECT_TRUE(static_cast<bool>(lock));
    lock.unlock();
    EXPECT_FALSE(static_cast<bool>(lock));
}

TEST(UniqueLockRecursiveTest, LockableCompliance) {
    dynamic_recursive_mutex m;
    freertos::unique_lock<dynamic_recursive_mutex> lock(m);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(LockGuardCompatTest, WithMutex) {
    dynamic_mutex m;
    {
        freertos::lock_guard<dynamic_mutex> guard(m);
    }
}

TEST(LockGuardCompatTest, WithRecursiveMutex) {
    dynamic_recursive_mutex m;
    {
        freertos::lock_guard<dynamic_recursive_mutex> guard(m);
    }
}

#if FREERTOS_CPP_WRAPPERS_CPP20
TEST(ConceptTest, MutexIsBasicLockable) {
    static_assert(freertos::BasicLockable<dynamic_mutex>);
    static_assert(freertos::Lockable<dynamic_mutex>);
    static_assert(freertos::TimedLockable<dynamic_mutex>);
}

TEST(ConceptTest, RecursiveMutexIsTimedLockable) {
    static_assert(freertos::BasicLockable<dynamic_recursive_mutex>);
    static_assert(freertos::Lockable<dynamic_recursive_mutex>);
    static_assert(freertos::TimedLockable<dynamic_recursive_mutex>);
}

TEST(ConceptTest, SharedMutexIsSharedLockable) {
    static_assert(freertos::BasicLockable<dynamic_shared_mutex>);
    static_assert(freertos::Lockable<dynamic_shared_mutex>);
    static_assert(freertos::TimedLockable<dynamic_shared_mutex>);
    static_assert(freertos::SharedLockable<dynamic_shared_mutex>);
    static_assert(freertos::SharedTimedLockable<dynamic_shared_mutex>);
}
#endif

#endif
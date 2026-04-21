#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"
#include "freertos_concepts.hpp"

#if __cpp_concepts >= 201907L

struct valid_allocator {
  static void *allocate(size_t) { return nullptr; }
  static void deallocate(void *, size_t) {}
};

struct invalid_allocator_no_allocate {
  static void deallocate(void *, size_t) {}
};

struct invalid_allocator_wrong_return {
  static int allocate(size_t) { return 0; }
  static void deallocate(void *, size_t) {}
};

struct valid_waitable {
  void acquire(TickType_t) {}
};

struct valid_synchronizable : valid_waitable {
  void release() {}
};

struct invalid_no_acquire {
  void release() {}
};

struct valid_lockable {
  BaseType_t lock() { return pdTRUE; }
  BaseType_t unlock() { return pdTRUE; }
};

struct valid_timed_lockable : valid_lockable {
  bool try_lock_for(std::chrono::milliseconds) { return true; }
};

struct invalid_no_unlock {
  BaseType_t lock() { return pdTRUE; }
};

struct valid_callback {
  void operator()() {}
};

struct invalid_not_invocable {
  int data;
};

TEST(FreertosConcepts, FreeRTOSAllocatorSatisfied) {
  static_assert(freertos::FreeRTOSAllocator<valid_allocator>);
}

TEST(FreertosConcepts, FreeRTOSAllocatorNotSatisfiedNoAllocate) {
  static_assert(!freertos::FreeRTOSAllocator<invalid_allocator_no_allocate>);
}

TEST(FreertosConcepts, FreeRTOSAllocatorNotSatisfiedWrongReturn) {
  static_assert(!freertos::FreeRTOSAllocator<invalid_allocator_wrong_return>);
}

TEST(FreertosConcepts, WaitableSatisfied) {
  static_assert(freertos::Waitable<valid_waitable>);
}

TEST(FreertosConcepts, WaitableNotSatisfiedNoAcquire) {
  static_assert(!freertos::Waitable<invalid_no_acquire>);
}

TEST(FreertosConcepts, SynchronizableSatisfied) {
  static_assert(freertos::Synchronizable<valid_synchronizable>);
}

TEST(FreertosConcepts, SynchronizableNotSatisfied) {
  static_assert(!freertos::Synchronizable<valid_waitable>);
}

TEST(FreertosConcepts, LockableSatisfied) {
  static_assert(freertos::Lockable<valid_lockable>);
}

TEST(FreertosConcepts, LockableNotSatisfiedNoUnlock) {
  static_assert(!freertos::Lockable<invalid_no_unlock>);
}

TEST(FreertosConcepts, TimedLockableSatisfied) {
  static_assert(freertos::TimedLockable<valid_timed_lockable>);
}

TEST(FreertosConcepts, TimedLockableNotSatisfied) {
  static_assert(!freertos::TimedLockable<valid_lockable>);
}

TEST(FreertosConcepts, TaskCallbackSatisfied) {
  static_assert(freertos::TaskCallback<valid_callback>);
  static_assert(freertos::TaskCallback<void (*)()>);
}

TEST(FreertosConcepts, TaskCallbackNotSatisfied) {
  static_assert(!freertos::TaskCallback<invalid_not_invocable>);
}

TEST(FreertosConcepts, TimerCallbackSatisfied) {
  static_assert(freertos::TimerCallback<valid_callback>);
  static_assert(freertos::TimerCallback<void (*)()>);
}

TEST(FreertosConcepts, TimerCallbackNotSatisfied) {
  static_assert(!freertos::TimerCallback<invalid_not_invocable>);
}

#if configSUPPORT_DYNAMIC_ALLOCATION
TEST(FreertosConcepts, BinarySemaphoreIsWaitable) {
  static_assert(freertos::Waitable<freertos::da::binary_semaphore>);
}

TEST(FreertosConcepts, BinarySemaphoreIsSynchronizable) {
  static_assert(freertos::Synchronizable<freertos::da::binary_semaphore>);
}

TEST(FreertosConcepts, CountingSemaphoreIsWaitable) {
  static_assert(freertos::Waitable<freertos::da::counting_semaphore>);
}

TEST(FreertosConcepts, CountingSemaphoreIsSynchronizable) {
  static_assert(freertos::Synchronizable<freertos::da::counting_semaphore>);
}

TEST(FreertosConcepts, MutexIsLockable) {
  static_assert(freertos::Lockable<freertos::da::mutex>);
}

TEST(FreertosConcepts, MutexIsTimedLockable) {
  static_assert(freertos::TimedLockable<freertos::da::mutex>);
}

TEST(FreertosConcepts, RecursiveMutexIsLockable) {
  static_assert(freertos::Lockable<freertos::da::recursive_mutex>);
}

TEST(FreertosConcepts, RecursiveMutexIsTimedLockable) {
  static_assert(freertos::TimedLockable<freertos::da::recursive_mutex>);
}
#endif

#else

TEST(FreertosConcepts, ConceptsUnavailable) {}

#endif
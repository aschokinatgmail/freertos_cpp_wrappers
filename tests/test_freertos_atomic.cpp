/**
 * @file test_freertos_atomic.cpp
 * @brief Unit tests for freertos::atomic_flag and freertos::atomic<T>
 *
 * Naming convention: TestSuiteName_TestCaseName
 *
 * Tests cover:
 * - atomic_flag: clear, test_and_set, test, wait/notify
 * - atomic<T>: basic store/load, CAS, wait/notify_one/notify_all
 * - ISR variants
 * - Static allocation variants
 * - C++ named requirement compliance checks
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_atomic.hpp"

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

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

class AtomicFlagTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA0001);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(AtomicFlagTest, DefaultStateIsFalse) {
  freertos::atomic_flag f;
  EXPECT_FALSE(f.test());
}

TEST_F(AtomicFlagTest, TestAndSetReturnsFalseInitially) {
  freertos::atomic_flag f;
  EXPECT_FALSE(f.test_and_set());
  EXPECT_TRUE(f.test());
}

TEST_F(AtomicFlagTest, TestAndSetReturnsTrueWhenSet) {
  freertos::atomic_flag f;
  f.test_and_set();
  EXPECT_TRUE(f.test_and_set());
}

TEST_F(AtomicFlagTest, ClearResetsFlag) {
  freertos::atomic_flag f;
  f.test_and_set();
  f.clear();
  EXPECT_FALSE(f.test());
}

TEST_F(AtomicFlagTest, NotifyOneCreatesSemaphore) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();
}

TEST_F(AtomicFlagTest, NotifyAllCreatesSemaphore) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_all();
}

TEST_F(AtomicFlagTest, WaitReturnsImmediatelyWhenDifferent) {
  freertos::atomic_flag f;
  f.test_and_set();
  auto result = f.wait(false);
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFlagTest, NotifyOneIsrWithoutSemaphore) {
  freertos::atomic_flag f;
  auto result = f.notify_one_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(AtomicFlagTest, NotifyOneIsrWithSemaphore) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  auto result = f.notify_one_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(AtomicFlagTest, NotifyAllIsrWithoutSemaphore) {
  freertos::atomic_flag f;
  auto result = f.notify_all_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(AtomicFlagTest, NotifyAllIsrWithSemaphore) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_all();

  EXPECT_CALL(*mock, uxQueueMessagesWaitingFromISR(
      reinterpret_cast<QueueHandle_t>(mock_semaphore_handle)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .Times(FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  auto result = f.notify_all_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(AtomicFlagTest, NotifyOneExSuccess) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  auto result = f.notify_one_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFlagTest, NotifyOneExSemaphoreError) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  auto result = f.notify_one_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(AtomicFlagTest, NotifyAllExSuccess) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  auto result = f.notify_all_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFlagTest, NotifyAllExPartialFailure) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  auto result = f.notify_all_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFlagTest, SemaphoreCreatedOnlyOnce) {
  freertos::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(1 + FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();
  f.notify_all();
}

class AtomicTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
};

TEST_F(AtomicTest, DefaultConstructionZero) {
  freertos::atomic<int> a;
  EXPECT_EQ(a.load(), 0);
}

TEST_F(AtomicTest, ValueConstruction) {
  freertos::atomic<int> a(42);
  EXPECT_EQ(a.load(), 42);
}

TEST_F(AtomicTest, StoreAndLoad) {
  freertos::atomic<int> a(0);
  a.store(10);
  EXPECT_EQ(a.load(), 10);
}

TEST_F(AtomicTest, Exchange) {
  freertos::atomic<int> a(5);
  auto old = a.exchange(10);
  EXPECT_EQ(old, 5);
  EXPECT_EQ(a.load(), 10);
}

TEST_F(AtomicTest, CompareExchangeStrongSuccess) {
  freertos::atomic<int> a(5);
  int expected = 5;
  bool success = a.compare_exchange_strong(expected, 10);
  EXPECT_TRUE(success);
  EXPECT_EQ(expected, 5);
  EXPECT_EQ(a.load(), 10);
}

TEST_F(AtomicTest, CompareExchangeStrongFailure) {
  freertos::atomic<int> a(5);
  int expected = 3;
  bool success = a.compare_exchange_strong(expected, 10);
  EXPECT_FALSE(success);
  EXPECT_EQ(expected, 5);
  EXPECT_EQ(a.load(), 5);
}

TEST_F(AtomicTest, CompareExchangeWeakSuccess) {
  freertos::atomic<int> a(5);
  int expected = 5;
  bool success = a.compare_exchange_weak(expected, 10);
  EXPECT_TRUE(success);
  EXPECT_EQ(a.load(), 10);
}

TEST_F(AtomicTest, FetchAdd) {
  freertos::atomic<int> a(10);
  auto old = a.fetch_add(5);
  EXPECT_EQ(old, 10);
  EXPECT_EQ(a.load(), 15);
}

TEST_F(AtomicTest, FetchSub) {
  freertos::atomic<int> a(10);
  auto old = a.fetch_sub(3);
  EXPECT_EQ(old, 10);
  EXPECT_EQ(a.load(), 7);
}

TEST_F(AtomicTest, WaitReturnsImmediatelyWhenDifferent) {
  freertos::atomic<int> a(0);
  a.store(42);
  a.wait(0);
}

TEST_F(AtomicTest, NotifyOneNoop) { freertos::atomic<int> a(0); a.notify_one(); }

TEST_F(AtomicTest, NotifyAllNoop) { freertos::atomic<int> a(0); a.notify_all(); }

TEST_F(AtomicTest, NotifyOneIsrNoop) {
  freertos::atomic<int> a(0);
  auto result = a.notify_one_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(AtomicTest, NotifyAllIsrNoop) {
  freertos::atomic<int> a(0);
  auto result = a.notify_all_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(AtomicTest, NotifyOneExSuccess) {
  freertos::atomic<int> a(0);
  auto result = a.notify_one_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicTest, NotifyAllExSuccess) {
  freertos::atomic<int> a(0);
  auto result = a.notify_all_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicTest, AtomicBoolTypedef) {
  freertos::atomic_bool b(true);
  EXPECT_TRUE(b.load());
}

TEST_F(AtomicTest, AtomicIntTypedef) {
  freertos::atomic_int i(42);
  EXPECT_EQ(i.load(), 42);
}

TEST_F(AtomicTest, AtomicSizeTTypedef) {
  freertos::atomic_size_t s(100);
  EXPECT_EQ(s.load(), 100u);
}

TEST_F(AtomicTest, TriviallyCopyableRequirement) {
  static_assert(std::is_trivially_copyable<freertos::atomic<int>>::value,
                "atomic<int> must be trivially copyable");
}

TEST_F(AtomicTest, IsStandardLayout) {
  static_assert(std::is_standard_layout<freertos::atomic<int>>::value,
                "atomic<int> must be standard layout");
}

class AtomicBoolSpecificTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
};

TEST_F(AtomicBoolSpecificTest, StoreAndLoad) {
  freertos::atomic<bool> a(false);
  a.store(true);
  EXPECT_TRUE(a.load());
}

TEST_F(AtomicBoolSpecificTest, Exchange) {
  freertos::atomic<bool> a(false);
  auto old = a.exchange(true);
  EXPECT_FALSE(old);
  EXPECT_TRUE(a.load());
}

TEST_F(AtomicBoolSpecificTest, WaitReturnsImmediatelyWhenDifferent) {
  freertos::atomic<bool> a(false);
  a.store(true);
  a.wait(false);
}

class AtomicPtrdiffTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
};

TEST_F(AtomicPtrdiffTest, StoreAndLoad) {
  freertos::atomic_ptrdiff_t a(0);
  a.store(100);
  EXPECT_EQ(a.load(), 100);
}

#if configSUPPORT_STATIC_ALLOCATION

class StaticAtomicFlagTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA0002);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(StaticAtomicFlagTest, DefaultStateIsFalse) {
  freertos::sa::atomic_flag f;
  EXPECT_FALSE(f.test());
}

TEST_F(StaticAtomicFlagTest, TestAndSetReturnsFalseInitially) {
  freertos::sa::atomic_flag f;
  EXPECT_FALSE(f.test_and_set());
  EXPECT_TRUE(f.test());
}

TEST_F(StaticAtomicFlagTest, ClearResetsFlag) {
  freertos::sa::atomic_flag f;
  f.test_and_set();
  f.clear();
  EXPECT_FALSE(f.test());
}

TEST_F(StaticAtomicFlagTest, NotifyOneCreatesSemaphore) {
  freertos::sa::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();
}

TEST_F(StaticAtomicFlagTest, NotifyOneIsrWithoutSemaphore) {
  freertos::sa::atomic_flag f;
  auto result = f.notify_one_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StaticAtomicFlagTest, NotifyOneIsrWithSemaphore) {
  freertos::sa::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  auto result = f.notify_one_isr();
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(StaticAtomicFlagTest, WaitReturnsImmediatelyWhenDifferent) {
  freertos::sa::atomic_flag f;
  f.test_and_set();
  auto result = f.wait(false);
  EXPECT_TRUE(result.has_value());
}

TEST_F(StaticAtomicFlagTest, SemaphoreCreatedOnlyOnce) {
  freertos::sa::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .Times(1 + FREERTOS_CPP_WRAPPERS_ATOMIC_FLAG_MAX_WAITERS)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();
  f.notify_all();
}

#endif // configSUPPORT_STATIC_ALLOCATION

#if configSUPPORT_DYNAMIC_ALLOCATION

class DaAtomicFlagTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA0003);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(DaAtomicFlagTest, DefaultStateIsFalse) {
  freertos::da::atomic_flag f;
  EXPECT_FALSE(f.test());
}

TEST_F(DaAtomicFlagTest, TestAndSetAndClear) {
  freertos::da::atomic_flag f;
  EXPECT_FALSE(f.test_and_set());
  EXPECT_TRUE(f.test());
  f.clear();
  EXPECT_FALSE(f.test());
}

TEST_F(DaAtomicFlagTest, NotifyOneWithSemaphore) {
  freertos::da::atomic_flag f;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
  f.notify_one();
}

#endif // configSUPPORT_DYNAMIC_ALLOCATION
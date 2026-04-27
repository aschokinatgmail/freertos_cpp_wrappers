#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"
#include "freertos_shared_data.hpp"
#include "freertos_condition_variable.hpp"
#include "freertos_latch.hpp"

#include <stdexcept>
#include <chrono>
#include <mutex>

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class ThrowingCopy {
public:
  int value = 0;
  static bool copy_throws;
  static bool assign_throws;
  static bool move_assign_throws;

  ThrowingCopy() = default;
  explicit ThrowingCopy(int v) : value(v) {}

  ThrowingCopy(const ThrowingCopy &other) : value(other.value) {
    if (copy_throws) {
      throw std::runtime_error("copy ctor throws");
    }
  }

  ThrowingCopy &operator=(const ThrowingCopy &other) {
    value = other.value;
    if (assign_throws) {
      throw std::runtime_error("copy assign throws");
    }
    return *this;
  }

  ThrowingCopy(ThrowingCopy &&other) : value(other.value) {}

  ThrowingCopy &operator=(ThrowingCopy &&other) {
    value = other.value;
    if (move_assign_throws) {
      throw std::runtime_error("move assign throws");
    }
    return *this;
  }
};

bool ThrowingCopy::copy_throws = false;
bool ThrowingCopy::assign_throws = false;
bool ThrowingCopy::move_assign_throws = false;

class ResetThrowingFlags {
public:
  ResetThrowingFlags() {
    ThrowingCopy::copy_throws = false;
    ThrowingCopy::assign_throws = false;
    ThrowingCopy::move_assign_throws = false;
  }
  ~ResetThrowingFlags() {
    ThrowingCopy::copy_throws = false;
    ThrowingCopy::assign_throws = false;
    ThrowingCopy::move_assign_throws = false;
  }
};

// ================================================================
// SHARED_DATA: exception safety catch blocks
// ================================================================

class SharedDataExceptionTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    ResetThrowingFlags();
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
    ThrowingCopy::copy_throws = false;
    ThrowingCopy::assign_throws = false;
    ThrowingCopy::move_assign_throws = false;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle =
      reinterpret_cast<SemaphoreHandle_t>(0xABCDEFFF);
};

TEST_F(SharedDataExceptionTest, GetThrowsOnCopy) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(42));
  ThrowingCopy::copy_throws = true;
  EXPECT_THROW(data.get(), std::runtime_error);
}

TEST_F(SharedDataExceptionTest, SetThrowsOnAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(10));
  ThrowingCopy::assign_throws = true;
  ThrowingCopy val(42);
  EXPECT_THROW(data.set(val), std::runtime_error);
}

TEST_F(SharedDataExceptionTest, SetMoveThrowsOnMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(10));
  ThrowingCopy::move_assign_throws = true;
  ThrowingCopy val(42);
  EXPECT_THROW(data.set(std::move(val)), std::runtime_error);
}

TEST_F(SharedDataExceptionTest, UseLambdaThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(5);
  EXPECT_THROW(data.use([](int &) { throw std::runtime_error("test"); }),
               std::runtime_error);
}

TEST_F(SharedDataExceptionTest, GetExThrowsOnCopy) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(42));
  ThrowingCopy::copy_throws = true;
  EXPECT_THROW(data.get_ex(), std::runtime_error);
}

TEST_F(SharedDataExceptionTest, SetExThrowsOnAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(10));
  ThrowingCopy::assign_throws = true;
  ThrowingCopy val(42);
  EXPECT_THROW(data.set_ex(val), std::runtime_error);
}

TEST_F(SharedDataExceptionTest, SetExMoveThrowsOnMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<ThrowingCopy> data(ThrowingCopy(10));
  ThrowingCopy::move_assign_throws = true;
  ThrowingCopy val(42);
  EXPECT_THROW(data.set_ex(std::move(val)), std::runtime_error);
}

// ================================================================
// MUTEX::CLAIM() catch block
// ================================================================

class MutexClaimExceptionTest : public ::testing::Test {
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
  SemaphoreHandle_t mock_handle =
      reinterpret_cast<SemaphoreHandle_t>(0x87654321);
};

TEST_F(MutexClaimExceptionTest, MutexClaimLambdaThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::mutex m;
  EXPECT_THROW(m.claim([]() { throw std::runtime_error("test"); }),
               std::runtime_error);
}

TEST_F(MutexClaimExceptionTest, MutexClaimLambdaThrowsVoidReturn) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::mutex m;
  bool called = false;
  try {
    m.claim([&called]() {
      called = true;
      throw 42;
    });
  } catch (int) {
  }
  EXPECT_TRUE(called);
}

TEST_F(MutexClaimExceptionTest, RecursiveMutexClaimLambdaThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::recursive_mutex rm;
  EXPECT_THROW(rm.claim([]() { throw std::runtime_error("test"); }),
               std::runtime_error);
}

TEST_F(MutexClaimExceptionTest, RecursiveMutexClaimLambdaThrowsVoidReturn) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::recursive_mutex rm;
  bool called = false;
  try {
    rm.claim([&called]() {
      called = true;
      throw 42;
    });
  } catch (int) {
  }
  EXPECT_TRUE(called);
}

TEST_F(MutexClaimExceptionTest, MutexWithLockLambdaThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::mutex m;
  EXPECT_THROW(m.with_lock([]() { throw std::runtime_error("test"); }),
               std::runtime_error);
}

TEST_F(MutexClaimExceptionTest, RecursiveMutexWithLockLambdaThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::da::recursive_mutex rm;
  EXPECT_THROW(rm.with_lock([]() { throw std::runtime_error("test"); }),
               std::runtime_error);
}

// ================================================================
// CONDITION_VARIABLE: wait_until with past deadline
// These tests use std::mutex since they test the condition_variable_any
// with a non-FreeRTOS lock, but we need the FreeRTOS mock for the
// condition_variable's semaphore.
// ================================================================

TEST(ConditionVariablePastDeadline, WaitUntilPastDeadlineReturnsTimeout) {
  freertos::condition_variable_any cv;
  std::mutex m;
  m.lock();
  auto past = std::chrono::steady_clock::now() - std::chrono::hours(1);
  auto status = cv.wait_until(m, past);
  EXPECT_EQ(status, std::cv_status::timeout);
}

TEST(ConditionVariablePastDeadline, WaitUntilPredicatePastDeadlineTrue) {
  freertos::condition_variable_any cv;
  std::mutex m;
  m.lock();
  auto past = std::chrono::steady_clock::now() - std::chrono::hours(1);
  bool result = cv.wait_until(m, past, [] { return true; });
  EXPECT_TRUE(result);
}

TEST(ConditionVariablePastDeadline, WaitUntilPredicatePastDeadlineFalse) {
  freertos::condition_variable_any cv;
  std::mutex m;
  m.lock();
  auto past = std::chrono::steady_clock::now() - std::chrono::hours(1);
  bool result = cv.wait_until(m, past, [] { return false; });
  EXPECT_FALSE(result);
}

// ================================================================
// LATCH: edge cases for uncovered branches
// ================================================================

class LatchBranchTest : public ::testing::Test {
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
  SemaphoreHandle_t mock_handle =
      reinterpret_cast<SemaphoreHandle_t>(0x11111111);
};

TEST_F(LatchBranchTest, CountDownAlreadyZeroNoGive) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(1);
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  l.count_down(1);
  EXPECT_TRUE(l.try_wait());

  l.count_down(1);
  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchBranchTest, CountDownExAlreadyZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(1);
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  l.count_down(1);

  auto result = l.count_down_ex(1);
  EXPECT_TRUE(result.has_value());
}

TEST_F(LatchBranchTest, CountDownNegativeUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(3);
  l.count_down(-1);
  EXPECT_FALSE(l.try_wait());
}

TEST_F(LatchBranchTest, CountDownExNegativeUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(3);
  auto result = l.count_down_ex(-1);
  EXPECT_TRUE(result.has_value());
}

TEST_F(LatchBranchTest, CountDownIsrNegativeUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(3);
  auto result = l.count_down_isr(-1);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(LatchBranchTest, CountDownExIsrNegativeUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(3);
  auto result = l.count_down_ex_isr(-1);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(LatchBranchTest, CountDownByMoreThanRemainingClampsToZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::latch l(2);
  l.count_down(5);
  EXPECT_TRUE(l.try_wait());
}
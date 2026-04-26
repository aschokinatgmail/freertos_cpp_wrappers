/**
 * @file test_freertos_latch.cpp
 * @brief Unit tests for freertos::latch
 *
 * Naming convention: TestSuiteName_TestCaseName
 *
 * Tests cover:
 * - Dynamic allocation latch (freertos::latch)
 * - Static allocation latch (freertos::sa::latch_static<N>)
 * - count_down, try_wait, wait, arrive_and_wait
 * - ISR variants (count_down_isr, count_down_ex, count_down_ex_isr)
 * - Edge cases (expected=1, expected=5, count_down by more than 1)
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_latch.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

class LatchTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0x11111111);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(LatchTest, DynamicLatchConstruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);
}

TEST_F(LatchTest, DynamicLatchConstructionExpected1) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);
}

TEST_F(LatchTest, DynamicLatchTryWaitNotReady) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);
  EXPECT_FALSE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownAndWait) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_FALSE(l.try_wait());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownMultipleSteps) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);

  EXPECT_FALSE(l.try_wait());

  l.count_down(1);
  EXPECT_FALSE(l.try_wait());

  l.count_down(1);
  EXPECT_FALSE(l.try_wait());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownByMoreThanOne) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(5);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownPartialByMoreThanOne) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);

  l.count_down(3);
  EXPECT_FALSE(l.try_wait());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(2);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchArriveAndWait) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));

  l.arrive_and_wait(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchArriveAndWaitAlreadyZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  l.arrive_and_wait(0);
}

TEST_F(LatchTest, DynamicLatchWaitAlreadyZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  l.wait();
}

TEST_F(LatchTest, DynamicLatchCountDownIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));

  auto result = l.count_down_isr(1);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownIsrNotTriggered) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);

  auto result = l.count_down_isr(1);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);

  EXPECT_FALSE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatchCountDownExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  auto result = l.count_down_ex(1);
  EXPECT_TRUE(result.has_value());
}

TEST_F(LatchTest, DynamicLatchCountDownExSemaphoreError) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdFALSE));
  auto result = l.count_down_ex(1);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(LatchTest, DynamicLatchCountDownExNoTrigger) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);

  auto result = l.count_down_ex(1);
  EXPECT_TRUE(result.has_value());
}

TEST_F(LatchTest, DynamicLatchCountDownExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));

  auto result = l.count_down_ex_isr(1);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(LatchTest, DynamicLatchCountDownExIsrSemaphoreError) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));

  auto result = l.count_down_ex_isr(1);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(LatchTest, DynamicLatchCountDownExIsrNoTrigger) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);

  auto result = l.count_down_ex_isr(1);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(LatchTest, DynamicLatchMaxReturnsPtrdiffMax) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(1);
  EXPECT_EQ(l.max(), PTRDIFF_MAX);
}

TEST_F(LatchTest, DynamicLatch_WaitBlocksOnSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(3);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));

  l.wait();
}

TEST_F(LatchTest, DynamicLatch_WaitNullSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(nullptr));

  freertos::latch l(3);

  l.wait();
}

TEST_F(LatchTest, DynamicLatch_CountDownNullSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(nullptr));

  freertos::latch l(1);
  l.count_down(1);
  EXPECT_TRUE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatch_CountDownIsrNullSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(nullptr));

  freertos::latch l(1);
  auto result = l.count_down_isr(1);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(LatchTest, DynamicLatch_CountDownExNullSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(nullptr));

  freertos::latch l(1);
  auto result = l.count_down_ex(1);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(LatchTest, DynamicLatch_CountDownExIsrNullSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(nullptr));

  freertos::latch l(1);
  auto result = l.count_down_ex_isr(1);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(LatchTest, DynamicLatch_CountDownZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);
  l.count_down(0);
  EXPECT_FALSE(l.try_wait());
}

TEST_F(LatchTest, DynamicLatch_CountDownIsrZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);
  auto result = l.count_down_isr(0);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(LatchTest, DynamicLatch_CountDownExZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);
  auto result = l.count_down_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(LatchTest, DynamicLatch_CountDownExIsrZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::latch l(5);
  auto result = l.count_down_ex_isr(0);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

#if configSUPPORT_STATIC_ALLOCATION

class StaticLatchTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0x22222222);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(StaticLatchTest, Construction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<3> l;
}

TEST_F(StaticLatchTest, TryWaitNotReady) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<3> l;
  EXPECT_FALSE(l.try_wait());
}

TEST_F(StaticLatchTest, CountDownAndWait) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_FALSE(l.try_wait());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(StaticLatchTest, CountDownMultipleSteps) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<3> l;

  l.count_down(1);
  EXPECT_FALSE(l.try_wait());

  l.count_down(1);
  EXPECT_FALSE(l.try_wait());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(StaticLatchTest, ArriveAndWait) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));

  l.arrive_and_wait(1);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(StaticLatchTest, CountDownIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));

  auto result = l.count_down_isr(1);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);

  EXPECT_TRUE(l.try_wait());
}

TEST_F(StaticLatchTest, CountDownExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  auto result = l.count_down_ex(1);
  EXPECT_TRUE(result.has_value());
}

TEST_F(StaticLatchTest, WaitAlreadyZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));
  l.count_down(1);

  l.wait();
}

TEST_F(StaticLatchTest, MaxReturnsPtrdiffMax) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;
  EXPECT_EQ(l.max(), PTRDIFF_MAX);
}

TEST_F(StaticLatchTest, LatchAliasIsStatic) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch l;
  EXPECT_FALSE(l.try_wait());
}

TEST_F(StaticLatchTest, WaitBlocksOnSemaphore) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<3> l;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdTRUE));

  l.wait();
}

TEST_F(StaticLatchTest, CountDownSemaphoreGiveFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdFALSE));
  l.count_down(1);
}

TEST_F(StaticLatchTest, CountDownExSemaphoreGiveFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<1> l;

  EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
      .WillOnce(Return(pdFALSE));
  auto result = l.count_down_ex(1);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(StaticLatchTest, CountDownZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<5> l;
  l.count_down(0);
  EXPECT_FALSE(l.try_wait());
}

TEST_F(StaticLatchTest, CountDownIsrZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<5> l;
  auto result = l.count_down_isr(0);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StaticLatchTest, CountDownExZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<5> l;
  auto result = l.count_down_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(StaticLatchTest, CountDownExIsrZeroUpdate) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::sa::latch_static<5> l;
  auto result = l.count_down_ex_isr(0);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

#endif // configSUPPORT_STATIC_ALLOCATION

#if configSUPPORT_DYNAMIC_ALLOCATION

class DaLatchTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0x33333333);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_semaphore_handle;
};

TEST_F(DaLatchTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::da::latch l(2);
}

TEST_F(DaLatchTest, DaLatchIsSameAsFreertosLatch) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_semaphore_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));

  freertos::da::latch l(2);
  EXPECT_FALSE(l.try_wait());
}

#endif // configSUPPORT_DYNAMIC_ALLOCATION
/**********************************************************************************
@file test_freertos_queue_set.cpp
@author Assistant AI
@brief Comprehensive unit tests for FreeRTOS queue set wrapper
@version 3.1.0
@date 2026-04-17

The MIT License (MIT)

FreeRTOS C++ Wrappers Library Test Suite
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

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

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/freertos_mocks.hpp"

#include "freertos_queue_set.hpp"

#include "../include/freertos_isr_result.hpp"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;

class FreeRTOSQueueSetTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
    mock_queue_set_handle = reinterpret_cast<QueueSetHandle_t>(0x12345678);
    mock_member_handle = reinterpret_cast<QueueSetMemberHandle_t>(0xAAAABBBB);
  }

  void TearDown() override { FreeRTOSMockInstance::resetInstance(); }

  std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;
  QueueSetHandle_t mock_queue_set_handle;
  QueueSetMemberHandle_t mock_member_handle;
};

TEST_F(FreeRTOSQueueSetTest, DynamicQueueSetAllocatorCreate) {
  EXPECT_CALL(*mock, xQueueCreateSet(10))
      .WillOnce(Return(mock_queue_set_handle));

  freertos::dynamic_queue_set_allocator allocator;
  auto handle = allocator.create(10);
  EXPECT_EQ(handle, mock_queue_set_handle);
}

TEST_F(FreeRTOSQueueSetTest, DynamicQueueSetAllocatorCreateFailure) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(static_cast<QueueSetHandle_t>(nullptr)));

  freertos::dynamic_queue_set_allocator allocator;
  auto handle = allocator.create(5);
  EXPECT_EQ(handle, nullptr);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetConstruction) {
  EXPECT_CALL(*mock, xQueueCreateSet(10))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(10);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetConstructionWithAllocatorArgs) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs(5);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetAdd) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueAddToSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto rc = qs.add(mock_member_handle);
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetAddFail) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueAddToSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto rc = qs.add(mock_member_handle);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetRemove) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock,
              xQueueRemoveFromSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto rc = qs.remove(mock_member_handle);
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetRemoveFail) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock,
              xQueueRemoveFromSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto rc = qs.remove(mock_member_handle);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectWithTicks) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 100))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select(100);
  EXPECT_EQ(result, mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectWithTicksTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 100))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select(100);
  EXPECT_EQ(result, nullptr);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectChronoTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 2000))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select(std::chrono::seconds(2));
  EXPECT_EQ(result, mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectChronoMilliseconds) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 500))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select(std::chrono::milliseconds(500));
  EXPECT_EQ(result, mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectISR) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_isr();
  EXPECT_EQ(result.result, mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectISRNull) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_isr();
  EXPECT_EQ(result.result, nullptr);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetMoveConstruction) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(
      std::move(qs1));
}

TEST_F(FreeRTOSQueueSetTest, QueueSetMoveAssignment) {
  QueueSetHandle_t handle1 = reinterpret_cast<QueueSetHandle_t>(0x1111);
  QueueSetHandle_t handle2 = reinterpret_cast<QueueSetHandle_t>(0x2222);

  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle1));
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vQueueDelete(handle1));
  EXPECT_CALL(*mock, vQueueDelete(handle2));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(5);
  qs1 = std::move(qs2);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSwap) {
  QueueSetHandle_t handle1 = reinterpret_cast<QueueSetHandle_t>(0x1111);
  QueueSetHandle_t handle2 = reinterpret_cast<QueueSetHandle_t>(0x2222);

  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle1));
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vQueueDelete(handle1));
  EXPECT_CALL(*mock, vQueueDelete(handle2));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(5);
  qs1.swap(qs2);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetAddExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueAddToSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.add_ex(mock_member_handle);
  EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSQueueSetTest, QueueSetAddExFail) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueAddToSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.add_ex(mock_member_handle);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetRemoveExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock,
              xQueueRemoveFromSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.remove_ex(mock_member_handle);
  EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSQueueSetTest, QueueSetRemoveExFail) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock,
              xQueueRemoveFromSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.remove_ex(mock_member_handle);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 100))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 100))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 0))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExChronoSuccess) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 2000))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(std::chrono::seconds(2));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExChronoTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, 500))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(std::chrono::milliseconds(500));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExISRSuccess) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), mock_member_handle);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExISREmpty) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetSelectExISRWithWoken) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(
          [](QueueSetHandle_t, BaseType_t *woken) -> QueueSetMemberHandle_t {
            *woken = pdTRUE;
            return reinterpret_cast<QueueSetMemberHandle_t>(0xDEADBEEF);
          });
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetComplexScenario) {
  QueueSetMemberHandle_t member1 =
      reinterpret_cast<QueueSetMemberHandle_t>(0x1111);
  QueueSetMemberHandle_t member2 =
      reinterpret_cast<QueueSetMemberHandle_t>(0x2222);

  EXPECT_CALL(*mock, xQueueCreateSet(2))
      .WillOnce(Return(mock_queue_set_handle));

  InSequence seq;
  EXPECT_CALL(*mock, xQueueAddToSet(member1, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xQueueAddToSet(member2, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, portMAX_DELAY))
      .WillOnce(Return(member1));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member1, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(2);
  EXPECT_EQ(qs.add(member1), pdPASS);
  EXPECT_EQ(qs.add(member2), pdPASS);
  auto selected = qs.select(portMAX_DELAY);
  EXPECT_EQ(selected, member1);
  EXPECT_EQ(qs.remove(member1), pdPASS);
}

TEST_F(FreeRTOSQueueSetTest, QueueSetDestructionNullHandle) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(static_cast<QueueSetHandle_t>(nullptr)));

  freertos::da::queue_set qs(5);
}
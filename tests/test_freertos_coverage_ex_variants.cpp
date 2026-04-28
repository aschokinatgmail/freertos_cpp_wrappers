#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_event_group.hpp"
#include "freertos_sw_timer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;

class ExVariantsSemTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsSemTest, BinarySemaphoreGiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.give_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(ExVariantsSemTest, BinarySemaphoreGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.give_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex(100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex(std::chrono::milliseconds(200));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, BinarySemaphoreGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.give_ex_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(ExVariantsSemTest, BinarySemaphoreGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.give_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(ExVariantsSemTest, BinarySemaphoreTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, CountingSemaphoreGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.give_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExVariantsSemTest, CountingSemaphoreTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 50)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.take_ex(50);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, CountingSemaphoreTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.take_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, CountingSemaphoreTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.take_ex(std::chrono::milliseconds(300));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, MutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 50)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(50);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, MutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, MutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.unlock_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExVariantsSemTest, MutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

TEST_F(ExVariantsSemTest, MutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.unlock_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::semaphore_not_owned);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

TEST_F(ExVariantsSemTest, MutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.try_lock_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, RecursiveMutexLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, RecursiveMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsSemTest, RecursiveMutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_ex(std::chrono::milliseconds(250));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsSemTest, RecursiveMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.unlock_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExVariantsSemTest, RecursiveMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.try_lock_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

// Event Group _ex variants
class ExVariantsEventGroupTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x2001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsEventGroupTest, SetBitsExIsrSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(h, 0x01, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 0x01);
}

TEST_F(ExVariantsEventGroupTest, SetBitsExIsrFailure) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(h, 0x01, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.set_bits_ex_isr(0x01);
  EXPECT_FALSE(r.result.has_value());
  // pdFAIL from xEventGroupSetBitsFromISR signals the timer-service
  // daemon's command queue is full, not an invalid handle.
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsEventGroupTest, WaitBitsExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupWaitBits(h, 0x01, pdTRUE, pdTRUE, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsEventGroupTest, WaitBitsExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupWaitBits(h, 0x02, pdTRUE, pdTRUE, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.wait_bits_ex(0x02, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsEventGroupTest, SyncExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSync(h, 0x01, 0x03, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.sync_ex(0x01, 0x03, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsEventGroupTest, SyncExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSync(h, 0x01, 0x03, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// Queue _ex variants
class ExVariantsQueueTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x3001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsQueueTest, SendExQueueFullZeroTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(ExVariantsQueueTest, SendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_ex(42, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsQueueTest, SendExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_ex(42, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsQueueTest, SendBackExQueueFull) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_back_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(ExVariantsQueueTest, SendBackExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_back_ex(42, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsQueueTest, SendFrontExQueueFull) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_front_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(ExVariantsQueueTest, SendFrontExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_front_ex(42, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsQueueTest, ReceiveExQueueEmpty) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int val;
  auto r = q.receive_ex(val, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_empty);
}

TEST_F(ExVariantsQueueTest, ReceiveExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int val;
  auto r = q.receive_ex(val, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsQueueTest, ReceiveExOptionalWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.receive_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_empty);
}

TEST_F(ExVariantsQueueTest, OverwriteExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwrite(h, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.overwrite_ex(42);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

// Stream Buffer _ex variants
class ExVariantsStreamBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x4001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsStreamBufferTest, SendExWouldBlock) {
  // Non-blocking send returned 0 bytes but the buffer is not full.
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_ex("test", 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, SendExBufferFull) {
  // Non-blocking send returned 0 bytes because the buffer is full.
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_ex("test", 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::buffer_full);
}

TEST_F(ExVariantsStreamBufferTest, SendExChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_ex("test", 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsStreamBufferTest, SendExIteratorWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  const char data[] = "test";
  auto r = sb.send_ex(data, data + 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, SendExIteratorChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  const char data[] = "test";
  auto r = sb.send_ex(data, data + 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsStreamBufferTest, ReceiveExWouldBlock) {
  // Non-blocking receive returned 0 bytes but the buffer is not empty.
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceive(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, ReceiveExBufferEmpty) {
  // Non-blocking receive returned 0 bytes because the buffer is empty.
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceive(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::buffer_empty);
}

TEST_F(ExVariantsStreamBufferTest, ReceiveExChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceive(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsStreamBufferTest, SendExIsrWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_ex_isr("test", 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, SendExIsrBufferFull) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_ex_isr("test", 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_full);
}

TEST_F(ExVariantsStreamBufferTest, SendExIsrIteratorWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  const char data[] = "test";
  auto r = sb.send_ex_isr(data, data + 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, ReceiveExIsrWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsStreamBufferTest, ReceiveExIsrBufferEmpty) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_empty);
}

TEST_F(ExVariantsStreamBufferTest, ResetExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReset(h)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

TEST_F(ExVariantsStreamBufferTest, SetTriggerLevelExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(h, 1)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.set_trigger_level_ex(1);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_parameter);
}

// Message Buffer _ex variants
class ExVariantsMessageBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x5001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsMessageBufferTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_ex("test", 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsMessageBufferTest, SendExBufferFull) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsFull(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_ex("test", 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::buffer_full);
}

TEST_F(ExVariantsMessageBufferTest, SendExMessageTooLarge) {
  // data_size + sizeof(size_t) > MessageBufferSize triggers message_too_large
  // up front, before invoking xMessageBufferSend.
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<32, freertos::dynamic_message_buffer_allocator<32>> mb;
  // 32 - 4 = 28 max payload; pass 64 bytes to exceed capacity.
  char big[64] = {};
  auto r = mb.send_ex(big, sizeof(big), 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::message_too_large);
}

TEST_F(ExVariantsMessageBufferTest, SendExChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_ex("test", 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsMessageBufferTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceive(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(ExVariantsMessageBufferTest, ReceiveExBufferEmpty) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceive(h, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::buffer_empty);
}

TEST_F(ExVariantsMessageBufferTest, ReceiveExChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceive(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(ExVariantsMessageBufferTest, SendExIsrWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsFull(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_ex_isr("test", 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsMessageBufferTest, SendExIsrBufferFull) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsFull(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_ex_isr("test", 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_full);
}

TEST_F(ExVariantsMessageBufferTest, SendExIsrMessageTooLarge) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<32, freertos::dynamic_message_buffer_allocator<32>> mb;
  char big[64] = {};
  auto r = mb.send_ex_isr(big, sizeof(big));
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::message_too_large);
}

TEST_F(ExVariantsMessageBufferTest, ReceiveExIsrWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(ExVariantsMessageBufferTest, ReceiveExIsrBufferEmpty) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_empty);
}

TEST_F(ExVariantsMessageBufferTest, ResetExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReset(h)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// Timer _ex variants
class ExVariantsTimerTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x6001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExVariantsTimerTest, StartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStart(h, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.start_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, StartExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStart(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.start_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, StartExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStartFromISR(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.start_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  // pdFAIL from xTimerStartFromISR means the timer command queue is full,
  // not that the handle is invalid (which would be caught by the pre-check).
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, StopExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStop(h, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.stop_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, StopExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStopFromISR(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.stop_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, ResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerReset(h, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, ResetExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerResetFromISR(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.reset_ex_isr();
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, PeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriod(h, 200, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex(200);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, PeriodExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(h, 200, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex_isr(200);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, PeriodExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriod(h, _, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex(std::chrono::milliseconds(200), std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timer_queue_full);
}

TEST_F(ExVariantsTimerTest, PeriodExIsrChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(h, _, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::timer<freertos::dynamic_sw_timer_allocator> t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex_isr(std::chrono::milliseconds(200));
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::timer_queue_full);
}
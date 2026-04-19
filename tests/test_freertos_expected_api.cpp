#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_event_group.hpp"
#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_queue.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_sw_timer.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

class ExpectedApiTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_sem = reinterpret_cast<SemaphoreHandle_t>(0x11111111);
    mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x22222222);
    mock_queue = reinterpret_cast<QueueHandle_t>(0x33333333);
    mock_timer = reinterpret_cast<TimerHandle_t>(0x44444444);
    mock_eg = reinterpret_cast<EventGroupHandle_t>(0x55555555);
    mock_sb = reinterpret_cast<StreamBufferHandle_t>(0x66666666);
    mock_mb = reinterpret_cast<MessageBufferHandle_t>(0x77777777);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_sem;
  SemaphoreHandle_t mock_mutex_handle;
  QueueHandle_t mock_queue;
  TimerHandle_t mock_timer;
  EventGroupHandle_t mock_eg;
  StreamBufferHandle_t mock_sb;
  MessageBufferHandle_t mock_mb;
};

// ---------- binary_semaphore _ex ----------

TEST_F(ExpectedApiTest, BinarySemaphoreGiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.give_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, BinarySemaphoreGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExpectedApiTest, BinarySemaphoreTakeExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.take_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, BinarySemaphoreTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedApiTest, BinarySemaphoreTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedApiTest, BinarySemaphoreTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, BinarySemaphoreGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(ExpectedApiTest, BinarySemaphoreTakeExIsrWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::binary_semaphore sem;
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

// ---------- mutex _ex ----------

TEST_F(ExpectedApiTest, MutexLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedApiTest, MutexUnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  mtx.lock();
  auto result = mtx.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, MutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ExpectedApiTest, MutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, MutexTryLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedApiTest, MutexLockExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(ExpectedApiTest, MutexLockExIsrWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

// ---------- queue _ex ----------

TEST_F(ExpectedApiTest, QueueSendExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, QueueSendExQueueFull) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 0))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(ExpectedApiTest, QueueSendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 100))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedApiTest, QueueSendExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(ExpectedApiTest, QueueSendExIsrQueueFull) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(ExpectedApiTest, QueueReceiveExSuccess) {
  int val = 99;
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(val, 0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, QueueReceiveExQueueEmpty) {
  int val;
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(ExpectedApiTest, QueueResetExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReset(mock_queue)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.reset_ex();
  EXPECT_TRUE(result.has_value());
}

// ---------- timer _ex ----------

TEST_F(ExpectedApiTest, TimerStartExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, TimerStartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ExpectedApiTest, TimerStopExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, TimerStartExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(ExpectedApiTest, TimerResetExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerReset(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, TimerPeriodExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, 200, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex(200);
  EXPECT_TRUE(result.has_value());
}

// ---------- event_group _ex ----------

TEST_F(ExpectedApiTest, EventGroupSetBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSetBits(mock_eg, 0x01))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x01});
}

TEST_F(ExpectedApiTest, EventGroupWaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, _))
      .WillOnce(Return(EventBits_t{0x03}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

TEST_F(ExpectedApiTest, EventGroupWaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, 100))
      .WillOnce(Return(EventBits_t{0x00}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedApiTest, EventGroupClearBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupClearBits(mock_eg, 0x01))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.clear_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, EventGroupSyncExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSync(mock_eg, 0x01, 0x03, _))
      .WillOnce(Return(EventBits_t{0x03}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, EventGroupSyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSync(mock_eg, 0x01, 0x03, 100))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

// ---------- stream_buffer _ex ----------

TEST_F(ExpectedApiTest, StreamBufferSendExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, _, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, 0);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(ExpectedApiTest, StreamBufferSendExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, _, 0))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedApiTest, StreamBufferReceiveExSuccess) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_sb, _, _, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex(buf, 4, 0);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(ExpectedApiTest, StreamBufferResetExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_sb)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.reset_ex();
  EXPECT_TRUE(result.has_value());
}

// ---------- message_buffer _ex ----------

TEST_F(ExpectedApiTest, MessageBufferSendExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(mock_mb, _, _, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex(data, 4, 0);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(ExpectedApiTest, MessageBufferSendExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(mock_mb, _, _, 0))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex(data, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedApiTest, MessageBufferResetExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(mock_mb)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.reset_ex();
  EXPECT_TRUE(result.has_value());
}

// ---------- recursive_mutex _ex ----------

TEST_F(ExpectedApiTest, RecursiveMutexLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_sem, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.lock_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedApiTest, RecursiveMutexTryLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_sem, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}
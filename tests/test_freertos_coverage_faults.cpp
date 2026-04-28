#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <stream_buffer.h>
#include <message_buffer.h>
#include <stdexcept>

#include "freertos_task.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_queue_set.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;
using ::testing::StrEq;
using ::testing::Throw;
using ::testing::SaveArg;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
} // namespace

// ================================================================
// QUEUE: Constructor with registered name + destructor name check
// ================================================================

class QueueNameTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x7001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueNameTest, ConstructorWithRegisteredName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, StrEq("myqueue")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return("myqueue"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q("myqueue");
}

TEST_F(QueueNameTest, DestructorWithNullName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
}

TEST_F(QueueNameTest, MoveConstructNullHandleDestructor) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x7010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, pcQueueGetName(h1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  freertos::da::queue<10, int> a;
  freertos::da::queue<10, int> b;
}

// ================================================================
// QUEUE: _ex timeout paths and ISR success paths
// ================================================================

class QueueFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x7002);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueFaultTest, SendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, SendBackExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, SendFrontExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, ReceiveExRefTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, ReceiveExOptionalTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, PeekExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueFaultTest, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendFromISR(fake_q, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueFaultTest, SendBackExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_q, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueFaultTest, SendFrontExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_q, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueFaultTest, ReceiveIsrVoidNoData) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_q, _, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.receive_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueFaultTest, PeekIsrNoData) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeekFromISR(fake_q, _, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.peek_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueFaultTest, OverwriteExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueOverwrite(fake_q, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.overwrite_ex(42);
  EXPECT_FALSE(result.has_value());
}

// ================================================================
// EVENT GROUP: _ex timeout paths + ISR success + move-from-null dtor
// ================================================================

class EventGroupFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x2001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupFaultTest, WaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupFaultTest, SyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupFaultTest, SetBitsExIsrSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(result.result.has_value());
}

// ================================================================
// SEMAPHORE: _ex timeout paths + ISR success + guards + try_lock_until
// ================================================================

class SemaphoreFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemaphoreFaultTest, BinarySemaphoreTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SemaphoreFaultTest, BinarySemaphoreGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreFaultTest, BinarySemaphoreTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreFaultTest, MutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SemaphoreFaultTest, MutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemaphoreFaultTest, MutexUnlockExSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  m.lock();
  auto result = m.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreFaultTest, MutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemaphoreFaultTest, MutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.try_lock_ex();
  EXPECT_TRUE(result.has_value());
  m.unlock();
}

TEST_F(SemaphoreFaultTest, MutexTryLockGuardSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  {
    freertos::try_lock_guard<freertos::da::mutex> guard(m);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreFaultTest, MutexTimeoutLockGuardSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  {
    freertos::timeout_lock_guard<freertos::da::mutex> guard(m, 100);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreFaultTest, MutexTimeoutLockGuardChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  {
    freertos::timeout_lock_guard<freertos::da::mutex> guard(
        m, std::chrono::milliseconds(50));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreFaultTest, RecursiveMutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto result = m.lock_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SemaphoreFaultTest, RecursiveMutexUnlockExSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  m.lock();
  auto result = m.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreFaultTest, RecursiveMutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto result = m.try_lock_ex();
  EXPECT_TRUE(result.has_value());
  m.unlock();
}

// ================================================================
// STREAM BUFFER: _ex timeout paths + ISR success paths
// ================================================================

class StreamBufferFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x4001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(StreamBufferFaultTest, SendExTimeout) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, 100)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.send_ex("test", 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(StreamBufferFaultTest, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.send_ex_isr("test", 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 4u);
}

TEST_F(StreamBufferFaultTest, ReceiveExTimeout) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, 100)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  auto result = sb.receive_ex(buf, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(StreamBufferFaultTest, ReceiveExIsrSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  auto result = sb.receive_ex_isr(buf, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 4u);
}

TEST_F(StreamBufferFaultTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReset(fake_sb)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(StreamBufferFaultTest, SetTriggerLevelExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(fake_sb, 1)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.set_trigger_level_ex(1);
  EXPECT_TRUE(result.has_value());
}

// ================================================================
// MESSAGE BUFFER: _ex timeout paths + ISR success paths
// ================================================================

class MessageBufferFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x3001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferFaultTest, SendExTimeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, 100)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  auto result = mb.send_ex("test", 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferFaultTest, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  auto result = mb.send_ex_isr("test", 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 4u);
}

TEST_F(MessageBufferFaultTest, ReceiveExTimeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, 100)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex(buf, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferFaultTest, ReceiveExIsrSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex_isr(buf, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 4u);
}

TEST_F(MessageBufferFaultTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(fake_mb)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  auto result = mb.reset_ex();
  EXPECT_TRUE(result.has_value());
}

// ================================================================
// TIMER: _ex success paths + null handle paths
// ================================================================

class TimerFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x1000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TimerFaultTest, StartExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerFaultTest, StartExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerFaultTest, StopExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerFaultTest, StopExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerFaultTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerFaultTest, ResetExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerFaultTest, PeriodExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, 200, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex(200);
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerFaultTest, PeriodExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, 200, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(200);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerFaultTest, NullHandleStartReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start(), pdFAIL);
}

TEST_F(TimerFaultTest, NullHandleStartIsrReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start_isr().result, pdFAIL);
}

TEST_F(TimerFaultTest, NullHandleStopReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop(), pdFAIL);
}

TEST_F(TimerFaultTest, NullHandleStopIsrReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop_isr().result, pdFAIL);
}

TEST_F(TimerFaultTest, NullHandleResetReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset(), pdFAIL);
}

TEST_F(TimerFaultTest, NullHandleResetIsrReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset_isr().result, pdFAIL);
}

TEST_F(TimerFaultTest, NullHandlePeriodReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(200), pdFAIL);
}

TEST_F(TimerFaultTest, NullHandlePeriodIsrReturnsFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_isr(200).result, pdFAIL);
}

TEST_F(TimerFaultTest, NullHandlePeriodTicksReturnsZero) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), 0);
}

TEST_F(TimerFaultTest, NullHandleRemainingTicksReturnsZero) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), 0);
}

TEST_F(TimerFaultTest, NullHandleRunningReturnsFalse) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_FALSE(t.running());
}

TEST_F(TimerFaultTest, NullHandleNameReturnsNull) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.name(), nullptr);
}

TEST_F(TimerFaultTest, NullHandleReloadModeNoop) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.reload_mode(pdTRUE);
}

// ================================================================
// QUEUE SET: add_ex/remove_ex success + select_ex_isr success
// ================================================================

class QueueSetFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueSetHandle_t fake_qs = reinterpret_cast<QueueSetHandle_t>(0x8001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueSetFaultTest, AddExSuccess) {
  QueueHandle_t member = reinterpret_cast<QueueHandle_t>(0x8010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueAddToSet(member, fake_qs)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.add_ex(member);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueSetFaultTest, RemoveExSuccess) {
  QueueHandle_t member = reinterpret_cast<QueueHandle_t>(0x8010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member, fake_qs)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.remove_ex(member);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueSetFaultTest, SelectExIsrSuccess) {
  QueueHandle_t member = reinterpret_cast<QueueHandle_t>(0x8010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(fake_qs, _)).WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

// ================================================================
// TASK: is_running with different states + null handle tests
// ================================================================

class TaskFaultTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskFaultTest, IsRunningReadyState) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eReady));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_running());
}

TEST_F(TaskFaultTest, IsRunningBlockedState) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eBlocked));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_running());
}

TEST_F(TaskFaultTest, IsAliveInvalidState) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eInvalid));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_alive());
}

TEST_F(TaskFaultTest, AbortDelayNullHandle) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.abort_delay();
  EXPECT_EQ(result, pdPASS);
}

TEST_F(TaskFaultTest, MoveAssignFromNullSkipsDelete) {
  testing::InSequence seq;
  TaskHandle_t t1_handle = reinterpret_cast<TaskHandle_t>(0x3010);
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(t1_handle), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(static_cast<TaskHandle_t>(nullptr)),
                      Return(pdFAIL)));
  EXPECT_CALL(*mock, vTaskDelete(t1_handle));

  freertos::da::task<512> t1("t1", 1, []() {});
  freertos::da::task<512> t2("t2", 1, []() {});
  t1 = std::move(t2);
}

TEST_F(TaskFaultTest, NotifyTakeChronoThrows) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyTake(_, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_THROW((void)t.notify_take(pdTRUE, std::chrono::milliseconds(10)),
               std::runtime_error);
}

TEST_F(TaskFaultTest, NotifyWaitChronoThrows) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(_, _, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  uint32_t value = 0;
  EXPECT_THROW((void)t.notify_wait(0, 0, value, std::chrono::milliseconds(10)),
               std::runtime_error);
}

TEST_F(TaskFaultTest, NotifyIsrThrows) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyFromISR(fake_task, _, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_THROW((void)t.notify_isr(1, eSetValueWithOverwrite),
               std::runtime_error);
}

TEST_F(TaskFaultTest, NotifyAndQueryIsrThrows) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyAndQueryFromISR(fake_task, _, _, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  uint32_t prev = 0;
  EXPECT_THROW((void)t.notify_and_query_isr(1, eSetValueWithOverwrite, prev),
               std::runtime_error);
}

TEST_F(TaskFaultTest, DelayFreeFunctionThrows) {
  EXPECT_CALL(*mock, vTaskDelay(_)).WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_THROW((void)freertos::delay(std::chrono::milliseconds(1)),
               std::runtime_error);
}

TEST_F(TaskFaultTest, DelayUntilFreeFunctionThrows) {
  TickType_t previous = 0;
  EXPECT_CALL(*mock, vTaskDelayUntil(_, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_THROW(
      (void)freertos::delay_until(previous, std::chrono::milliseconds(1)),
      std::runtime_error);
}

// ================================================================
// EXTERNAL ALLOCATOR: allocation failure paths
// ================================================================

class MockRegion {
public:
  int call_count = 0;
  int fail_after = -1;
  void *allocate(size_t) noexcept {
    call_count++;
    if (fail_after >= 0 && call_count > fail_after) {
      return nullptr;
    }
    return reinterpret_cast<void *>(0x1000 + call_count * 0x100);
  }
  void deallocate(void *) noexcept {}
};

TEST(ExternalAllocatorTest, SemaphoreCreateBinaryFailure) {
  MockRegion region;
  region.fail_after = 0;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_binary();
  EXPECT_EQ(result, nullptr);
}

TEST(ExternalAllocatorTest, SemaphoreCreateMutexFailure) {
  MockRegion region;
  region.fail_after = 0;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_mutex();
  EXPECT_EQ(result, nullptr);
}

TEST(ExternalAllocatorTest, SemaphoreCreateRecursiveMutexFailure) {
  MockRegion region;
  region.fail_after = 0;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_recursive_mutex();
  EXPECT_EQ(result, nullptr);
}

TEST(ExternalAllocatorTest, EventGroupAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_event_group_allocator<MockRegion> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, StreamBufferStructAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
  auto result = alloc.create(1);
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, StreamBufferStorageAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 1;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
  auto result = alloc.create(1);
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, MessageBufferStructAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, MessageBufferStorageAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 1;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, SwTimerAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_sw_timer_allocator<MockRegion> alloc(region);
  auto result = alloc.create("t", 100, pdTRUE, nullptr, dummy_timer_callback);
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, TaskStructAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_task_allocator<MockRegion, 512> alloc(region);
  auto result = alloc.create(nullptr, "t", 1, nullptr);
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, TaskStackAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 1;
  freertos::external_task_allocator<MockRegion, 512> alloc(region);
  auto result = alloc.create(nullptr, "t", 1, nullptr);
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, QueueStructAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 0;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, QueueStorageAllocateFailure) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  region.fail_after = 1;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
  delete nice_mock;
  g_freertos_mock = nullptr;
}
// ================================================================
// ADDITIONAL BRANCH DIRECTION TESTS
// Targeting: would_block/queue_full (zero timeout) paths,
// _ex success paths (return value), ISR result TRUE directions,
// move-from-null destructor paths, and remaining uncovered branches
// ================================================================

// --- QUEUE: would_block (timeout=0) paths ---

TEST_F(QueueFaultTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueFaultTest, SendBackExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueFaultTest, SendFrontExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueFaultTest, ReceiveExRefWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueFaultTest, ReceiveExOptionalWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueFaultTest, PeekExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

// --- QUEUE: _ex success paths (pdPASS return) ---

TEST_F(QueueFaultTest, SendExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, portMAX_DELAY);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueFaultTest, ReceiveExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, portMAX_DELAY);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueFaultTest, OverwriteExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueOverwrite(fake_q, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.overwrite_ex(42);
  EXPECT_TRUE(result.has_value());
}

// --- QUEUE: name-related tests ---

TEST_F(QueueFaultTest, ConstructorWithoutNameDestructorNoName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
}

struct QueueAllocatorArg {
  int token;
};

class QueueArgAllocator {
public:
  explicit QueueArgAllocator(QueueAllocatorArg arg) : m_arg(arg) {}
  QueueHandle_t create() { return xQueueCreate(10, sizeof(int)); }

private:
  QueueAllocatorArg m_arg;
};

TEST_F(QueueFaultTest, ConstructorWithAllocatorArgsNoName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  QueueAllocatorArg arg{1};
  freertos::queue<10, int, QueueArgAllocator> q(nullptr, arg);
}

TEST_F(QueueFaultTest, ConstructorWithAllocatorArgsAndName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, StrEq("qname")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return("qname"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  QueueAllocatorArg arg{2};
  freertos::queue<10, int, QueueArgAllocator> q("qname", arg);
}

TEST_F(QueueFaultTest, ReceiveIsrOptionalThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.receive_isr(), std::runtime_error);
}

TEST_F(QueueFaultTest, SendExIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_ex_isr(42), std::runtime_error);
}

TEST_F(QueueFaultTest, SendBackExIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_back_ex_isr(42), std::runtime_error);
}

TEST_F(QueueFaultTest, SendFrontExIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_front_ex_isr(42), std::runtime_error);
}

TEST_F(QueueFaultTest, ReceiveExOptionalSuccessValue) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(portMAX_DELAY);
  EXPECT_TRUE(result.has_value());
}

// --- EVENT GROUP: would_block (timeout=0) paths ---

TEST_F(EventGroupFaultTest, WaitBitsExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupFaultTest, SyncExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// --- EVENT GROUP: self-move assignment path ---

class EventGroupMoveTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x2002);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupMoveTest, SelfMove) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  eg = std::move(eg);
}

struct EventGroupAllocatorArg {
  int token;
};

class EventGroupArgAllocator {
public:
  explicit EventGroupArgAllocator(EventGroupAllocatorArg arg) : m_arg(arg) {}
  EventGroupHandle_t create() { return xEventGroupCreate(); }

private:
  EventGroupAllocatorArg m_arg;
};

TEST_F(EventGroupMoveTest, ConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  EventGroupAllocatorArg arg{42};
  freertos::event_group<EventGroupArgAllocator> eg(arg);
}

// --- SEMAPHORE: would_block (timeout=0) paths ---

TEST_F(SemaphoreFaultTest, BinarySemaphoreTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreFaultTest, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreFaultTest, RecursiveMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto result = m.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// --- SEMAPHORE: give_ex success paths ---

TEST_F(SemaphoreFaultTest, BinarySemaphoreGiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.give_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreGiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.give_ex();
  EXPECT_TRUE(result.has_value());
}

// --- SEMAPHORE: lock_ex/try_lock_until success paths ---

TEST_F(SemaphoreFaultTest, MutexTryLockUntilSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.try_lock_until(std::chrono::system_clock::now() + std::chrono::hours(1));
  EXPECT_TRUE(result);
  m.unlock();
}

TEST_F(SemaphoreFaultTest, MutexTryLockUntilFailure) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  EXPECT_FALSE(m.try_lock_until(std::chrono::system_clock::now() + std::chrono::milliseconds(10)));
}

// --- SEMAPHORE: move-from-null destructor paths ---

class SemaphoreMoveFromNullTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5002);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemaphoreMoveFromNullTest, BinarySemaphoreMoveThenDestroy) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore a;
  freertos::da::binary_semaphore b = std::move(a);
}

TEST_F(SemaphoreMoveFromNullTest, MutexMoveThenDestroy) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex a;
  freertos::da::mutex b = std::move(a);
}

TEST_F(SemaphoreMoveFromNullTest, CountingSemaphoreMoveThenDestroy) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore a(5);
  freertos::da::counting_semaphore b = std::move(a);
}

struct SemAllocatorArg {
  int token;
};

class SemArgAllocator {
public:
  explicit SemArgAllocator(SemAllocatorArg arg) : m_arg(arg) {}
  SemaphoreHandle_t create_binary() { return xSemaphoreCreateBinary(); }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    return xSemaphoreCreateCounting(max_count, max_count);
  }
  SemaphoreHandle_t create_mutex() { return xSemaphoreCreateMutex(); }
  SemaphoreHandle_t create_recursive_mutex() {
    return xSemaphoreCreateRecursiveMutex();
  }

private:
  SemAllocatorArg m_arg;
};

class SemArgCtorTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5A01);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemArgCtorTest, BinarySemaphoreWithAllocatorArgs) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  SemAllocatorArg arg{1};
  freertos::binary_semaphore<SemArgAllocator> bs(arg);
}

TEST_F(SemArgCtorTest, CountingSemaphoreWithAllocatorArgs) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(3, 3)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  SemAllocatorArg arg{2};
  freertos::counting_semaphore<SemArgAllocator> cs(3, arg);
}

TEST_F(SemArgCtorTest, MutexWithAllocatorArgs) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  SemAllocatorArg arg{3};
  freertos::mutex<SemArgAllocator> m(arg);
}

TEST_F(SemArgCtorTest, RecursiveMutexWithAllocatorArgs) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  SemAllocatorArg arg{4};
  freertos::recursive_mutex<SemArgAllocator> m(arg);
}

TEST_F(SemArgCtorTest, MutexTryLockUntilExpiredFalse) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto tp = std::chrono::system_clock::now() - std::chrono::milliseconds(1);
  EXPECT_FALSE(m.try_lock_until(tp));
}

TEST_F(SemArgCtorTest, MutexTryLockUntilFuturePath) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto tp = std::chrono::system_clock::now() + std::chrono::milliseconds(20);
  EXPECT_TRUE(m.try_lock_until(tp));
  m.unlock();
}

TEST_F(SemArgCtorTest, LockGuardIsrConstructorFailurePath) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  freertos::lock_guard_isr<freertos::da::mutex> guard(m);
  EXPECT_FALSE(guard.locked());
}

// --- STREAM BUFFER: would_block paths ---

TEST_F(StreamBufferFaultTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.send_ex("test", 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferFaultTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  auto result = sb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// --- MESSAGE BUFFER: would_block paths ---

TEST_F(MessageBufferFaultTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  auto result = mb.send_ex("test", 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferFaultTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// --- TIMER: _ex failure paths ---

TEST_F(TimerFaultTest, StartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerFaultTest, StopExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerFaultTest, ResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerFaultTest, PeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, 200, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex(200);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

// --- TIMER: null handle _ex failure paths ---

TEST_F(TimerFaultTest, NullHandleStartExReturnsError) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerFaultTest, NullHandleStopExReturnsError) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

// --- TIMER: move assignment ---

class TimerMoveTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x1001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

struct TimerAllocatorArg {
  int token;
};

class TimerArgAllocator {
public:
  explicit TimerArgAllocator(TimerAllocatorArg arg) : m_arg(arg) {}
  TimerHandle_t create(const char *name, const TickType_t period_ticks,
                       UBaseType_t auto_reload, void *const timer_id,
                       TimerCallbackFunction_t callback) {
    return xTimerCreate(name, period_ticks, auto_reload, timer_id, callback);
  }

private:
  TimerAllocatorArg m_arg;
};

TEST_F(TimerMoveTest, ConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  TimerAllocatorArg arg{1};
  freertos::timer<TimerArgAllocator> t("t", 100, pdTRUE, []() {}, arg);
}

TEST_F(TimerMoveTest, ChronoConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  TimerAllocatorArg arg{2};
  freertos::timer<TimerArgAllocator> t(
      "t", std::chrono::milliseconds(100), pdTRUE, []() {}, arg);
}

TEST_F(TimerMoveTest, MoveAssignDestinationNullHandle) {
  testing::InSequence seq;
  TimerHandle_t src_handle = reinterpret_cast<TimerHandle_t>(0x1111);
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(src_handle));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerStop(src_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(src_handle)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(src_handle)).WillOnce(Return(50));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(src_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(src_handle, portMAX_DELAY));

  freertos::da::timer src("src", 50, pdTRUE, []() {});
  freertos::da::timer dst("dst", 50, pdTRUE, []() {});
  dst = std::move(src);
}

TEST_F(TimerMoveTest, SwapWithNullOther) {
  testing::InSequence seq;
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x2222);
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vTimerSetTimerID(h, _));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::da::timer a("a", 10, pdTRUE, []() {});
  freertos::da::timer b("b", 10, pdTRUE, []() {});
  a.swap(b);
}

TEST_F(TimerMoveTest, PeriodChronoDelegationThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.period(std::chrono::milliseconds(200),
                              std::chrono::milliseconds(20)),
               std::runtime_error);
}

TEST_F(TimerMoveTest, MoveAssignmentNotStarted) {
  testing::InSequence seq;
  TimerHandle_t timer2 = reinterpret_cast<TimerHandle_t>(0x1002);
  TimerHandle_t new_timer = reinterpret_cast<TimerHandle_t>(0x1003);
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(timer2));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(fake_timer)).WillOnce(Return("t1"));
  EXPECT_CALL(*mock, xTimerGetPeriod(fake_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(fake_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(new_timer));
  EXPECT_CALL(*mock, xTimerDelete(timer2, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(new_timer, portMAX_DELAY));
  freertos::da::timer t1("t1", 100, pdTRUE, []() {});
  freertos::da::timer t2("t2", 100, pdTRUE, []() {});
  t2 = std::move(t1);
}

// --- TASK: move-from-null destructor ---

class TaskMoveFromNullTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskMoveFromNullTest, TaskMoveThenDestroy) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> a("t", 1, []() {});
  freertos::da::task<512> b = std::move(a);
}

// --- QUEUE SET: null handle destructor path ---

class QueueSetNullHandleTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueSetNullHandleTest, NullHandleDestructor) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(nullptr));
  freertos::da::queue_set qs(10);
}

// ================================================================
// CHRONO TEMPLATE DELEGATION TESTS
// Exercise std::chrono::duration overloads to cover template branches
// ================================================================

// --- QUEUE chrono delegations ---

class QueueChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x7003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueChronoTest, SendChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_EQ(q.send(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(QueueChronoTest, SendChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendBackChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_EQ(q.send_back(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(QueueChronoTest, SendFrontChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_EQ(q.send_front(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(QueueChronoTest, ReceiveChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  EXPECT_EQ(q.receive(val, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(QueueChronoTest, ReceiveOptionalChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto opt = q.receive(std::chrono::milliseconds(100));
  EXPECT_TRUE(opt.has_value());
}

TEST_F(QueueChronoTest, PeekChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  EXPECT_EQ(q.peek(val, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(QueueChronoTest, SendExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, SendBackExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, SendFrontExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, ReceiveExRefChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, ReceiveExOptChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, PeekExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueChronoTest, SendBackChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_back(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendFrontChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_front(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, ReceiveChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.receive(val, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, ReceiveOptionalChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.receive(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, PeekChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.peek(val, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, PeekOptionalChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.peek(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendExChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_ex(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendBackExChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_back_ex(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendFrontExChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_front_ex(42, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, ReceiveExRefChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.receive_ex(val, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, ReceiveExOptChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.receive_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, PeekExChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.peek_ex(val, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueChronoTest, SendIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_isr(42), std::runtime_error);
}

TEST_F(QueueChronoTest, SendBackIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_back_isr(42), std::runtime_error);
}

TEST_F(QueueChronoTest, SendFrontIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.send_front_isr(42), std::runtime_error);
}

TEST_F(QueueChronoTest, ReceiveIsrRefThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.receive_isr(val), std::runtime_error);
}

TEST_F(QueueChronoTest, OverwriteIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueOverwriteFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.overwrite_isr(42), std::runtime_error);
}

TEST_F(QueueChronoTest, PeekIsrRefThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeekFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  int val = 0;
  EXPECT_THROW((void)q.peek_isr(val), std::runtime_error);
}

TEST_F(QueueChronoTest, PeekIsrOptThrows) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeekFromISR(fake_q, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
  EXPECT_THROW((void)q.peek_isr(), std::runtime_error);
}

// --- SEMAPHORE chrono delegations ---

class SemChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemChronoTest, BinarySemaphoreTakeChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  EXPECT_TRUE(bs.take(std::chrono::milliseconds(100)));
}

TEST_F(SemChronoTest, BinarySemaphoreTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemChronoTest, CountingSemaphoreTakeChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  EXPECT_TRUE(cs.take(std::chrono::milliseconds(100)));
}

TEST_F(SemChronoTest, CountingSemaphoreTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemChronoTest, MutexLockChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  m.lock(std::chrono::milliseconds(100));
  m.unlock();
}

TEST_F(SemChronoTest, MutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  m.unlock();
}

TEST_F(SemChronoTest, RecursiveMutexLockChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  m.lock(std::chrono::milliseconds(100));
  m.unlock();
}

TEST_F(SemChronoTest, RecursiveMutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto result = m.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  m.unlock();
}

TEST_F(SemChronoTest, BinarySemaphoreTakeChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  EXPECT_THROW((void)bs.take(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, BinarySemaphoreTakeExChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  EXPECT_THROW((void)bs.take_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, CountingSemaphoreTakeChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  EXPECT_THROW((void)cs.take(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, CountingSemaphoreTakeExChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  EXPECT_THROW((void)cs.take_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, MutexLockChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  EXPECT_THROW((void)m.lock(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, MutexLockExChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  EXPECT_THROW((void)m.lock_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, RecursiveMutexLockChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  EXPECT_THROW((void)m.lock(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, RecursiveMutexLockExChronoThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  EXPECT_THROW((void)m.lock_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(SemChronoTest, BinarySemaphoreGiveExIsrThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  EXPECT_THROW((void)bs.give_ex_isr(), std::runtime_error);
}

TEST_F(SemChronoTest, BinarySemaphoreTakeExIsrThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::binary_semaphore bs;
  EXPECT_THROW((void)bs.take_ex_isr(), std::runtime_error);
}

TEST_F(SemChronoTest, CountingSemaphoreGiveExIsrThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  EXPECT_THROW((void)cs.give_ex_isr(), std::runtime_error);
}

TEST_F(SemChronoTest, CountingSemaphoreTakeExIsrThrows) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  EXPECT_THROW((void)cs.take_ex_isr(), std::runtime_error);
}

TEST_F(SemChronoTest, MutexLockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemChronoTest, MutexUnlockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemChronoTest, MutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemChronoTest, MutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemChronoTest, RecursiveMutexTryLockUntilExpiredFalse) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto tp = std::chrono::system_clock::now() - std::chrono::milliseconds(1);
  EXPECT_FALSE(m.try_lock_until(tp));
}

TEST_F(SemChronoTest, RecursiveMutexTryLockUntilFuture) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto tp = std::chrono::system_clock::now() + std::chrono::milliseconds(50);
  EXPECT_TRUE(m.try_lock_until(tp));
  m.unlock();
}

TEST_F(SemChronoTest, MutexTryLockGuardNotLocked) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  freertos::try_lock_guard<freertos::da::mutex> guard(m);
  EXPECT_FALSE(guard.locked());
}

TEST_F(SemChronoTest, TimeoutLockGuardNotLocked) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  freertos::timeout_lock_guard<freertos::da::mutex> guard(
      m, std::chrono::milliseconds(20));
  EXPECT_FALSE(guard.locked());
}

// --- TIMER chrono delegations ---

class TimerChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x1000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TimerChronoTest, StartChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerChronoTest, StopChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerChronoTest, ResetChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerChronoTest, PeriodChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(std::chrono::milliseconds(200), std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerChronoTest, StartExChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerChronoTest, StopExChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerChronoTest, ResetExChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerChronoTest, PeriodExChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex(std::chrono::milliseconds(200), std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

// --- EVENT GROUP chrono delegations ---

class EventGroupChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x2003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupChronoTest, WaitBitsChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.wait_bits(0x01, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_EQ(result & 0x01, 0x01);
}

TEST_F(EventGroupChronoTest, WaitBitsExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(EventGroupChronoTest, SyncChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.sync(0x01, 0x03, std::chrono::milliseconds(100));
  EXPECT_EQ(result & 0x03, 0x03);
}

TEST_F(EventGroupChronoTest, SyncExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(EventGroupChronoTest, WaitBitsChronoThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.wait_bits(0x01, pdTRUE, pdTRUE,
                                  std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(EventGroupChronoTest, SyncChronoThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.sync(0x01, 0x03, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(EventGroupChronoTest, WaitBitsExChronoThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.wait_bits_ex(0x01, pdTRUE, pdTRUE,
                                     std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(EventGroupChronoTest, SyncExChronoThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(EventGroupChronoTest, SetBitsIsrThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.set_bits_isr(0x01), std::runtime_error);
}

TEST_F(EventGroupChronoTest, SetBitsExIsrThrows) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::da::event_group eg;
  EXPECT_THROW((void)eg.set_bits_ex_isr(0x01), std::runtime_error);
}

// --- STREAM BUFFER chrono delegations ---

class StreamBufferChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x4003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

struct StreamAllocatorArg {
  int token;
};

class StreamArgAllocator {
public:
  explicit StreamArgAllocator(StreamAllocatorArg arg) : m_arg(arg) {}
  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreate(64, trigger_level_bytes);
  }

private:
  StreamAllocatorArg m_arg;
};

TEST_F(StreamBufferChronoTest, ConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  StreamAllocatorArg arg{1};
  freertos::stream_buffer<64, StreamArgAllocator> sb(1, arg);
}

TEST_F(StreamBufferChronoTest, SendChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  EXPECT_EQ(sb.send("test", 4, std::chrono::milliseconds(100)), 4);
}

TEST_F(StreamBufferChronoTest, SendExChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.send_ex("test", 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4u);
}

TEST_F(StreamBufferChronoTest, ReceiveChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  EXPECT_EQ(sb.receive(buf, 4, std::chrono::milliseconds(100)), 4);
}

TEST_F(StreamBufferChronoTest, ReceiveExChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  auto result = sb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4u);
}

TEST_F(StreamBufferChronoTest, SendChronoThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  EXPECT_THROW((void)sb.send("test", 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(StreamBufferChronoTest, SendIsrThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  EXPECT_THROW((void)sb.send_isr("test", 4), std::runtime_error);
}

TEST_F(StreamBufferChronoTest, ReceiveChronoThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  EXPECT_THROW((void)sb.receive(buf, 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(StreamBufferChronoTest, ReceiveIsrThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  EXPECT_THROW((void)sb.receive_isr(buf, 4), std::runtime_error);
}

TEST_F(StreamBufferChronoTest, SendExChronoThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  EXPECT_THROW((void)sb.send_ex("test", 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(StreamBufferChronoTest, SendExIsrThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  EXPECT_THROW((void)sb.send_ex_isr("test", 4), std::runtime_error);
}

TEST_F(StreamBufferChronoTest, ReceiveExChronoThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  EXPECT_THROW((void)sb.receive_ex(buf, 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(StreamBufferChronoTest, ReceiveExIsrThrows) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::da::stream_buffer<64> sb(1);
  char buf[4];
  EXPECT_THROW((void)sb.receive_ex_isr(buf, 4), std::runtime_error);
}

// --- MESSAGE BUFFER chrono delegations ---

class MessageBufferChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x3003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

struct MessageAllocatorArg {
  int token;
};

class MessageArgAllocator {
public:
  explicit MessageArgAllocator(MessageAllocatorArg arg) : m_arg(arg) {}
  MessageBufferHandle_t create() { return xMessageBufferCreate(256); }

private:
  MessageAllocatorArg m_arg;
};

TEST_F(MessageBufferChronoTest, ConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  MessageAllocatorArg arg{1};
  freertos::message_buffer<256, MessageArgAllocator> mb(arg);
}

TEST_F(MessageBufferChronoTest, SendChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  EXPECT_EQ(mb.send("test", 4, std::chrono::milliseconds(100)), 4);
}

TEST_F(MessageBufferChronoTest, SendExChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  auto result = mb.send_ex("test", 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4u);
}

TEST_F(MessageBufferChronoTest, ReceiveChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  EXPECT_EQ(mb.receive(buf, 4, std::chrono::milliseconds(100)), 4);
}

TEST_F(MessageBufferChronoTest, ReceiveExChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4u);
}

TEST_F(MessageBufferChronoTest, SendChronoThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  EXPECT_THROW((void)mb.send("test", 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(MessageBufferChronoTest, ReceiveChronoThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  EXPECT_THROW((void)mb.receive(buf, 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(MessageBufferChronoTest, SendIsrThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  EXPECT_THROW((void)mb.send_isr("test", 4), std::runtime_error);
}

TEST_F(MessageBufferChronoTest, ReceiveIsrThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  EXPECT_THROW((void)mb.receive_isr(buf, 4), std::runtime_error);
}

TEST_F(MessageBufferChronoTest, SendExChronoThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  EXPECT_THROW((void)mb.send_ex("test", 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(MessageBufferChronoTest, ReceiveExChronoThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  EXPECT_THROW((void)mb.receive_ex(buf, 4, std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(MessageBufferChronoTest, SendExIsrThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  EXPECT_THROW((void)mb.send_ex_isr("test", 4), std::runtime_error);
}

TEST_F(MessageBufferChronoTest, ReceiveExIsrThrows) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 4, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::da::message_buffer<256> mb;
  char buf[4];
  EXPECT_THROW((void)mb.receive_ex_isr(buf, 4), std::runtime_error);
}

// --- QUEUE SET chrono delegations ---

class QueueSetChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueSetHandle_t fake_qs = reinterpret_cast<QueueSetHandle_t>(0x8003);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueSetChronoTest, SelectChrono) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.select(std::chrono::milliseconds(100));
  EXPECT_EQ(result, nullptr);
}

TEST_F(QueueSetChronoTest, SelectChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  EXPECT_THROW((void)qs.select(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueSetChronoTest, SelectIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(fake_qs, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  EXPECT_THROW((void)qs.select_isr(), std::runtime_error);
}

TEST_F(QueueSetChronoTest, SelectExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, 100)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.select_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueSetChronoTest, SelectExChronoThrows) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  EXPECT_THROW((void)qs.select_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(QueueSetChronoTest, SelectExIsrThrows) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(fake_qs, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  EXPECT_THROW((void)qs.select_ex_isr(), std::runtime_error);
}

// --- TASK free-function chrono helpers ---

class TaskFreeFuncChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskFreeFuncChronoTest, DelayChrono) {
  EXPECT_CALL(*mock, vTaskDelay(_));
  freertos::delay(std::chrono::milliseconds(5));
}

TEST_F(TaskFreeFuncChronoTest, DelayUntilChrono) {
  TickType_t prev = 0;
  EXPECT_CALL(*mock, vTaskDelayUntil(_, _));
  freertos::delay_until(prev, std::chrono::milliseconds(5));
}

// --- MUTEX try_lock_until expired time point ---

TEST_F(SemChronoTest, MutexTryLockUntilExpired) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto result = m.try_lock_until(std::chrono::system_clock::time_point(std::chrono::milliseconds(-1)));
  EXPECT_TRUE(result);
  m.unlock();
}

// --- SEMAPHORE: ISR result processing for give_ex_isr/take_ex_isr success (counting_semaphore) ---

TEST_F(SemaphoreFaultTest, CountingSemaphoreGiveExIsrFailure) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SemaphoreFaultTest, CountingSemaphoreTakeExIsrFailure) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::counting_semaphore cs(10);
  auto result = cs.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

// --- TIMER: ISR _ex success paths (already tested above) plus chrono _isr ---

TEST_F(TimerChronoTest, StartExIsrChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerChronoTest, StopExIsrChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerChronoTest, ResetExIsrChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerChronoTest, PeriodExIsrChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(std::chrono::milliseconds(200));
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerChronoTest, StartChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.start(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, StopChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.stop(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, ResetChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.reset(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, StartIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.start_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, StopIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.stop_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, ResetIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.reset_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, PeriodChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.period(std::chrono::milliseconds(200),
                              std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, PeriodIsrChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.period_isr(std::chrono::milliseconds(200)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, StartExChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.start_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, StopExChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.stop_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, ResetExChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.reset_ex(std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, StartExIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.start_ex_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, StopExIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.stop_ex_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, ResetExIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.reset_ex_isr(), std::runtime_error);
}

TEST_F(TimerChronoTest, PeriodExChronoThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.period_ex(std::chrono::milliseconds(200),
                                 std::chrono::milliseconds(100)),
               std::runtime_error);
}

TEST_F(TimerChronoTest, PeriodExIsrThrows) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(Throw(std::runtime_error("boom")));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_THROW((void)t.period_ex_isr(std::chrono::milliseconds(200)),
               std::runtime_error);
}

// --- EXTERNAL ALLOCATOR: move-from destructor paths ---

TEST(ExternalAllocatorTest, SemaphoreMoveFromNullDestructor) {
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto sem = alloc.create_binary();
  auto sem2 = std::move(alloc);
  // After move, alloc's m_memory should be null - destructor should not crash
}

TEST(ExternalAllocatorTest, QueueMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
  auto q = alloc.create();
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, EventGroupMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_event_group_allocator<MockRegion> alloc(region);
  auto eg = alloc.create();
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, StreamBufferMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
  auto sb = alloc.create(1);
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, MessageBufferMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
  auto mb = alloc.create();
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, SwTimerMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_sw_timer_allocator<MockRegion> alloc(region);
  auto timer = alloc.create("t", 100, pdTRUE, nullptr, dummy_timer_callback);
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

TEST(ExternalAllocatorTest, TaskMoveFromNullDestructor) {
  NiceMock<FreeRTOSMock> *nice_mock = new NiceMock<FreeRTOSMock>();
  g_freertos_mock = nice_mock;
  MockRegion region;
  freertos::external_task_allocator<MockRegion, 512> alloc(region);
  auto task = alloc.create(nullptr, "t", 1, nullptr);
  auto alloc2 = std::move(alloc);
  delete nice_mock;
  g_freertos_mock = nullptr;
}

// ================================================================
// TASK: indexed notification coverage (lines 598, 615, 624, 642)
// ================================================================

TEST_F(TaskFaultTest, IndexedNotifyTakeChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdTRUE, _)).WillOnce(Return(1));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  auto val = t.notify_take(1, pdTRUE, std::chrono::milliseconds(10));
  EXPECT_EQ(val, 1u);
}

TEST_F(TaskFaultTest, IndexedNotifyIsr) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(fake_task, 1, 42,
                                                eSetValueWithOverwrite, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  auto r = t.notify_isr(1, 42, eSetValueWithOverwrite);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(TaskFaultTest, IndexedNotifyAndQueryIsr) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexedFromISR(fake_task, 1, 42,
                                                        eSetValueWithOverwrite,
                                                        _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  uint32_t prev = 0;
  auto r = t.notify_and_query_isr(1, 42, eSetValueWithOverwrite, prev);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(TaskFaultTest, IndexedNotifyWaitChrono) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::da::task<512> t("t", 1, []() {});
  uint32_t value = 0;
  auto rc =
      t.notify_wait(1, 0u, 0u, value, std::chrono::milliseconds(10));
  EXPECT_EQ(rc, pdPASS);
}

// ================================================================
// TASK: xTaskCreate failure branch (line 93)
// ================================================================

TEST_F(TaskFaultTest, DynamicTaskCreateFailReturnsNull) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(static_cast<TaskHandle_t>(nullptr)),
                      Return(pdFAIL)));
  // No vTaskDelete expected since handle is null
  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.handle(), nullptr);
}

// ================================================================
// TASK: move-assign into moved-from task (line 282 false branch)
// ================================================================

TEST_F(TaskFaultTest, MoveAssignIntoMovedFromTaskSkipsDelete) {
  testing::InSequence seq;
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3010);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3020);
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h1), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h2), Return(pdPASS)));
  // After: t1(h1), t2(h2). Move t1→t3: t3(h1), t1(nullptr).
  // Assign t2→t1: t1.m_hTask==nullptr so no delete (false branch), swap →
  // t1(h2), t2(nullptr).
  // Destruction reverse order: t3 deletes h1, t2 no-op, t1 deletes h2
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  freertos::da::task<512> t1("t1", 1, []() {});
  freertos::da::task<512> t2("t2", 1, []() {});
  // Move t1's handle out — t1 is now moved-from (null handle)
  freertos::da::task<512> t3(std::move(t1));
  // Now t1.m_hTask == nullptr. Assign t2 into t1 → line 282 if(m_hTask) is
  // false
  t1 = std::move(t2);
  // t1 now has h2. t3 has h1. t2 is null.
}

// ================================================================
// TASK: periodic_task chrono constructor (line 726) and no-period
// constructor (line 799)
// ================================================================

TEST_F(TaskFaultTest, PeriodicTaskChronoConstructor) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  // periodic_task destructor calls abort_delay() then member task destructor
  // calls vTaskDelete
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::milliseconds(100));
}

TEST_F(TaskFaultTest, PeriodicTaskNoPeriodConstructor) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1, []() {}, []() {}, []() {});
}

// ================================================================
// TASK: static task suspend() (missing function coverage)
// ================================================================

TEST_F(TaskFaultTest, StaticTaskSuspend256) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskSuspend(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<256> t("t", 1, []() {});
  t.suspend();
}

TEST_F(TaskFaultTest, StaticTaskSuspend512) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskSuspend(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<512> t("t", 1, []() {});
  t.suspend();
}

// ================================================================
// TASK: periodic_task operator() — chrono double+millis & long+millis
// (missing function coverage)
// ================================================================

TEST_F(TaskFaultTest, StaticPeriodicTaskOperatorChronoDouble) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::duration<double, std::milli>(100.0));
}

TEST_F(TaskFaultTest, StaticPeriodicTaskOperatorChronoLong) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::duration<long, std::milli>(100));
}

TEST_F(TaskFaultTest, DynamicPeriodicTaskOperatorChronoLong) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::duration<long, std::milli>(100));
}

// ================================================================
// SEMAPHORE: mutex try_lock_until with past time_point (lines 798-802)
// ================================================================

class SemaphoreCoverageTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x4000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemaphoreCoverageTest, MutexTryLockUntilPastTimePoint) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  // try_lock_until with a past time_point → calls try_lock() which calls
  // lock(0)
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  // mutex destructor does NOT unlock — it directly deletes
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(past));
}

TEST_F(SemaphoreCoverageTest, MutexTryLockUntilFutureTimePoint) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  // try_lock_until with future time_point → calls lock(duration)
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  // mutex destructor does NOT unlock — it directly deletes
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(future));
}

TEST_F(SemaphoreCoverageTest, MutexTryLockUntilPastFails) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_FALSE(m.try_lock_until(past));
}

// ================================================================
// SEMAPHORE: recursive_mutex try_lock_until (lines 975-979)
// ================================================================

TEST_F(SemaphoreCoverageTest, RecursiveMutexTryLockUntilPastTimePoint) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  // try_lock_until with past → calls try_lock() which calls lock(0)
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdTRUE));
  // recursive_mutex destructor does NOT unlock — it directly deletes
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(rm.try_lock_until(past));
}

TEST_F(SemaphoreCoverageTest, RecursiveMutexTryLockUntilFutureTimePoint) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _))
      .WillOnce(Return(pdTRUE));
  // recursive_mutex destructor does NOT unlock — it directly deletes
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  EXPECT_TRUE(rm.try_lock_until(future));
}

TEST_F(SemaphoreCoverageTest, RecursiveMutexTryLockUntilPastFails) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_FALSE(rm.try_lock_until(past));
}

// ================================================================
// SEMAPHORE: try_lock_guard locked() false branch (line 1077)
// ================================================================

TEST_F(SemaphoreCoverageTest, TryLockGuardLockedFalseWhenNotAcquired) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  freertos::try_lock_guard<freertos::da::mutex> guard(m);
  EXPECT_FALSE(guard.locked());
}

// ================================================================
// SEMAPHORE: lock_guard_isr constructor (line 1099)
// ================================================================

TEST_F(SemaphoreCoverageTest, LockGuardIsrConstructorAndDestructor) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::lock_guard_isr<freertos::da::mutex> guard(m);
    EXPECT_FALSE(guard.locked());
  }
}

// ================================================================
// SEMAPHORE: timeout_lock_guard chrono constructor (line 1167)
// and locked() false branch (line 1191)
// ================================================================

TEST_F(SemaphoreCoverageTest, TimeoutLockGuardChronoSuccess) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::timeout_lock_guard<freertos::da::mutex> guard(
        m, std::chrono::milliseconds(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreCoverageTest, TimeoutLockGuardChronoFailure) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::timeout_lock_guard<freertos::da::mutex> guard(
        m, std::chrono::milliseconds(100));
    EXPECT_FALSE(guard.locked());
  }
}

// ================================================================
// SEMAPHORE: AllocatorArgs constructors (lines 187, 371, 614, 837)
// ================================================================
// Note: For dynamic allocator these are no-ops, but the static allocator
// variants with AllocatorArgs are the uncovered paths. We use the static
// versions via swap tests below which also cover the AllocatorArgs ctors.

// ================================================================
// SEMAPHORE: static allocator swap() — missing functions
// ================================================================

TEST_F(SemaphoreCoverageTest, StaticRecursiveMutexSwap) {
  SemaphoreHandle_t fake_sem2 = reinterpret_cast<SemaphoreHandle_t>(0x4010);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(fake_sem))
      .WillOnce(Return(fake_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);

  freertos::sa::recursive_mutex rm1;
  freertos::sa::recursive_mutex rm2;
  rm1.swap(rm2);
}

TEST_F(SemaphoreCoverageTest, StaticRecursiveMutexLocked) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::sa::recursive_mutex rm;
  EXPECT_FALSE(rm.locked());
}

TEST_F(SemaphoreCoverageTest, StaticCountingSemaphoreSwap) {
  SemaphoreHandle_t fake_sem2 = reinterpret_cast<SemaphoreHandle_t>(0x4010);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(fake_sem))
      .WillOnce(Return(fake_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);

  freertos::sa::counting_semaphore cs1;
  freertos::sa::counting_semaphore cs2;
  cs1.swap(cs2);
}

TEST_F(SemaphoreCoverageTest, StaticMutexSwap) {
  SemaphoreHandle_t fake_sem2 = reinterpret_cast<SemaphoreHandle_t>(0x4010);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(fake_sem))
      .WillOnce(Return(fake_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);

  freertos::sa::mutex m1;
  freertos::sa::mutex m2;
  m1.swap(m2);
}

// ================================================================
// OTHER MISSING SWAPS: event_group, message_buffer, queue,
// stream_buffer (static allocator)
// ================================================================

TEST_F(SemaphoreCoverageTest, StaticEventGroupSwap) {
  EventGroupHandle_t fake_eg1 = reinterpret_cast<EventGroupHandle_t>(0x5000);
  EventGroupHandle_t fake_eg2 = reinterpret_cast<EventGroupHandle_t>(0x5010);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_))
      .WillOnce(Return(fake_eg1))
      .WillOnce(Return(fake_eg2));
  EXPECT_CALL(*mock, vEventGroupDelete(_)).Times(2);

  freertos::sa::event_group eg1;
  freertos::sa::event_group eg2;
  eg1.swap(eg2);
}

TEST_F(SemaphoreCoverageTest, StaticStreamBufferSwap) {
  StreamBufferHandle_t fake_sb1 =
      reinterpret_cast<StreamBufferHandle_t>(0x6000);
  StreamBufferHandle_t fake_sb2 =
      reinterpret_cast<StreamBufferHandle_t>(0x6010);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _))
      .WillOnce(Return(fake_sb1))
      .WillOnce(Return(fake_sb2));
  EXPECT_CALL(*mock, vStreamBufferDelete(_)).Times(2);

  freertos::sa::stream_buffer<256> sb1(1);
  freertos::sa::stream_buffer<256> sb2(1);
  sb1.swap(sb2);
}

TEST_F(SemaphoreCoverageTest, StaticMessageBufferSwap) {
  MessageBufferHandle_t fake_mb1 =
      reinterpret_cast<MessageBufferHandle_t>(0x7000);
  MessageBufferHandle_t fake_mb2 =
      reinterpret_cast<MessageBufferHandle_t>(0x7010);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
      .WillOnce(Return(fake_mb1))
      .WillOnce(Return(fake_mb2));
  EXPECT_CALL(*mock, vMessageBufferDelete(_)).Times(2);

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2;
  mb1.swap(mb2);
}

TEST_F(SemaphoreCoverageTest, StaticQueueSwap) {
  QueueHandle_t fake_q1 = reinterpret_cast<QueueHandle_t>(0x8000);
  QueueHandle_t fake_q2 = reinterpret_cast<QueueHandle_t>(0x8010);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _))
      .WillOnce(Return(fake_q1))
      .WillOnce(Return(fake_q2));
  // Queue destructor calls pcQueueGetName before vQueueDelete
  EXPECT_CALL(*mock, pcQueueGetName(_)).WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(_)).Times(2);

  freertos::sa::queue<10, int> q1;
  freertos::sa::queue<10, int> q2;
  q1.swap(q2);
}

// ================================================================
// EXPECTED: expected(unexpected<E>&&) — line 111
// ================================================================

TEST(ExpectedCoverageTest, ConstructFromUnexpectedRvalue) {
  freertos::unexpected<freertos::error> unex(freertos::error::timeout);
  freertos::expected<std::string, freertos::error> e(std::move(unex));
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), freertos::error::timeout);
}

// ================================================================
// TASK: default constructor coverage (line 205)
// — covered by the xTaskCreate failure test above which exercises
//   m_allocator.create() returning nullptr
// ================================================================

// --- TASK: chrono delay ---

// Note: delay() and delay_until() are member functions that must be called from
// the task's own thread context. We cannot directly test them in the mock
// environment without actually running the task function. These template
// instantiations are covered implicitly by the existing task tests.

// ================================================================
// EXPECTED: copy-constructor with error value (line 133 false branch)
// ================================================================

TEST(ExpectedCoverageTest, CopyConstructWithError) {
  freertos::expected<std::string, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<std::string, freertos::error> e2(e1);
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

// ================================================================
// SEMAPHORE: exception branches in mutex try_lock_until (lines 799, 801)
// ================================================================

TEST_F(SemaphoreCoverageTest, MutexTryLockUntilPastThrows) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  // try_lock_until(past) → try_lock() → xSemaphoreTake throws
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0))
      .WillOnce(Throw(std::runtime_error("fault")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_THROW(m.try_lock_until(past), std::runtime_error);
}

TEST_F(SemaphoreCoverageTest, MutexTryLockUntilFutureThrows) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  // try_lock_until(future) → lock(duration) → xSemaphoreTake throws
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _))
      .WillOnce(Throw(std::runtime_error("fault")));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  EXPECT_THROW(m.try_lock_until(future), std::runtime_error);
}

// ================================================================
// TASK: xTaskCreate throws during task construction (lines 93 + 205)
// ================================================================

TEST_F(TaskFaultTest, TaskCreateThrowsException) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(Throw(std::runtime_error("create fault")));

  EXPECT_THROW(
      freertos::da::task<512>("t", 1, []() {}),
      std::runtime_error);
}

// ================================================================
// TASK: xTaskCreate throws during periodic_task no-period construction
// (lines 726 + 799)
// ================================================================

TEST_F(TaskFaultTest, PeriodicTaskCreateThrowsNoPeriod) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(Throw(std::runtime_error("create fault")));

  EXPECT_THROW(
      (freertos::periodic_task<freertos::dynamic_task_allocator<512>>(
          "pt", 1, []() {}, []() {}, []() {})),
      std::runtime_error);
}

// ================================================================
// TASK: periodic_task lambda invocation — cover run() lambda operator()
// for three specific template instantiations
// ================================================================

// 1. periodic_task<dynamic_task_allocator<512>> with duration<long, milli>
TEST_F(TaskFaultTest, DynamicPeriodicTaskLambdaInvokeLong) {
  TaskFunction_t captured_fn = nullptr;
  void *captured_ctx = nullptr;

  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_fn), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  // run() calls is_alive() → eTaskGetState returns eDeleted → loop exits
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  // destructor: abort_delay + delete
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  bool on_start_called = false;
  bool on_stop_called = false;

  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      []() {},
      std::chrono::duration<long, std::milli>(100),
      false); // start_suspended = false

  ASSERT_NE(captured_fn, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_fn(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(on_stop_called);
}

// 2. periodic_task<static_task_allocator<1024>> with duration<double, milli>
TEST_F(TaskFaultTest, StaticPeriodicTaskLambdaInvokeDouble) {
  TaskFunction_t captured_fn = nullptr;
  void *captured_ctx = nullptr;

  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_fn), SaveArg<3>(&captured_ctx),
                       Return(fake_task)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  bool on_start_called = false;
  bool on_stop_called = false;

  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      []() {},
      std::chrono::duration<double, std::milli>(100.0),
      false);

  ASSERT_NE(captured_fn, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_fn(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(on_stop_called);
}

// 3. periodic_task<static_task_allocator<1024>> with duration<long, milli>
TEST_F(TaskFaultTest, StaticPeriodicTaskLambdaInvokeLong) {
  TaskFunction_t captured_fn = nullptr;
  void *captured_ctx = nullptr;

  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_fn), SaveArg<3>(&captured_ctx),
                       Return(fake_task)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  bool on_start_called = false;
  bool on_stop_called = false;

  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      []() {},
      std::chrono::duration<long, std::milli>(100),
      false);

  ASSERT_NE(captured_fn, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_fn(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(on_stop_called);
}

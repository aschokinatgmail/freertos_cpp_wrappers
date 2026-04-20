#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_sw_timer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class SaBinarySemaphoreTest : public ::testing::Test {
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

TEST_F(SaBinarySemaphoreTest, ConstructionWithAllocatorArg) {
  freertos::static_semaphore_allocator alloc;
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::static_semaphore_allocator> sem(std::move(alloc));
}

TEST_F(SaBinarySemaphoreTest, GiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  auto r = sem.give_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaBinarySemaphoreTest, TakeExChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  auto r = sem.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaBinarySemaphoreTest, AcquireRelease) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  sem.acquire();
  sem.release();
}

TEST_F(SaBinarySemaphoreTest, AcquireChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  sem.acquire(std::chrono::milliseconds(100));
}

TEST_F(SaBinarySemaphoreTest, TryAcquireSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  EXPECT_TRUE(sem.try_acquire());
}

TEST_F(SaBinarySemaphoreTest, TryAcquireFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::binary_semaphore sem;
  EXPECT_FALSE(sem.try_acquire());
}

class SaCountingSemaphoreTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1101);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaCountingSemaphoreTest, GiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(3);
  auto r = sem.give_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaCountingSemaphoreTest, TakeExChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(3);
  auto r = sem.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaCountingSemaphoreTest, AcquireRelease) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(3);
  sem.acquire();
  sem.release();
}

TEST_F(SaCountingSemaphoreTest, AcquireChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(3);
  sem.acquire(std::chrono::milliseconds(100));
}

TEST_F(SaCountingSemaphoreTest, TryAcquire) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(3);
  EXPECT_FALSE(sem.try_acquire());
}

TEST_F(SaCountingSemaphoreTest, CountMethod) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(h)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::counting_semaphore sem(5);
  EXPECT_EQ(sem.count(), 3u);
}

TEST_F(SaCountingSemaphoreTest, ConstructionWithAllocator) {
  freertos::static_semaphore_allocator alloc;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::static_semaphore_allocator> sem(5, std::move(alloc));
}

class SaMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1201);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaMutexTest, LockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::mutex m;
  auto r = m.lock_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaMutexTest, UnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::mutex m;
  auto r = m.unlock_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaMutexTest, TryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::mutex m;
  auto r = m.try_lock_ex();
  EXPECT_TRUE(r.has_value());
  m.unlock();
}

TEST_F(SaMutexTest, LockExChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::mutex m;
  auto r = m.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaMutexTest, ConstructionWithAllocator) {
  freertos::static_semaphore_allocator alloc;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::static_semaphore_allocator> m(std::move(alloc));
}

class SaRecursiveMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1301);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaRecursiveMutexTest, LockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::recursive_mutex m;
  auto r = m.lock_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaRecursiveMutexTest, UnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::recursive_mutex m;
  auto r = m.unlock_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaRecursiveMutexTest, TryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::recursive_mutex m;
  auto r = m.try_lock_ex();
  EXPECT_TRUE(r.has_value());
  m.unlock();
}

TEST_F(SaRecursiveMutexTest, LockExChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::sa::recursive_mutex m;
  auto r = m.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaRecursiveMutexTest, ConstructionWithAllocator) {
  freertos::static_semaphore_allocator alloc;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<freertos::static_semaphore_allocator> m(std::move(alloc));
}

class SaStreamBufferTest : public ::testing::Test {
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

TEST_F(SaStreamBufferTest, SendExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::sa::stream_buffer<256> sb;
  auto r = sb.send_ex("test", 4);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(SaStreamBufferTest, ReceiveExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceive(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::sa::stream_buffer<256> sb;
  char buf[4];
  auto r = sb.receive_ex(buf, 4);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(SaStreamBufferTest, SendExIteratorSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::sa::stream_buffer<256> sb;
  const char data[] = "test";
  auto r = sb.send_ex(data, data + 4);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

class SaMessageBufferTest : public ::testing::Test {
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

TEST_F(SaMessageBufferTest, SendExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::sa::message_buffer<256> mb;
  auto r = mb.send_ex("test", 4, portMAX_DELAY);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(SaMessageBufferTest, ReceiveExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceive(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::sa::message_buffer<256> mb;
  char buf[4];
  auto r = mb.receive_ex(buf, 4, portMAX_DELAY);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(SaMessageBufferTest, SendExChronoFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::sa::message_buffer<256> mb;
  auto r = mb.send_ex("test", 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaMessageBufferTest, ReceiveExChronoFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceive(h, _, 4, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::sa::message_buffer<256> mb;
  char buf[4];
  auto r = mb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

class SaEventGroupTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x6001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaEventGroupTest, SetBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBits(h, 0x01)).WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  auto r = eg.set_bits_ex(0x01);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 0x01);
}

TEST_F(SaEventGroupTest, ClearBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupClearBits(h, 0x01)).WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  auto r = eg.clear_bits_ex(0x01);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 0x03);
}

TEST_F(SaEventGroupTest, WaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupWaitBits(h, 0x01, pdTRUE, pdTRUE, portMAX_DELAY))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  auto r = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, portMAX_DELAY);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 0x01);
}

TEST_F(SaEventGroupTest, SyncExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSync(h, 0x01, 0x03, portMAX_DELAY))
      .WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  auto r = eg.sync_ex(0x01, 0x03, portMAX_DELAY);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 0x03);
}

TEST_F(SaEventGroupTest, SetBitsIsrResult) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(h, 0x01, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  auto r = eg.set_bits_isr(0x01);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaEventGroupTest, ClearBitsReturnValue) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupClearBits(h, 0x02)).WillOnce(Return(static_cast<EventBits_t>(0x02)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  EXPECT_EQ(eg.clear_bits(0x02), 0x02);
}

TEST_F(SaEventGroupTest, WaitBitsReturnValue) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupWaitBits(h, 0x03, pdTRUE, pdFALSE, portMAX_DELAY))
      .WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  EXPECT_EQ(eg.wait_bits(0x03, pdTRUE, pdFALSE, portMAX_DELAY), 0x03);
}

TEST_F(SaEventGroupTest, GetBitsReturnValue) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupGetBits(h)).WillOnce(Return(static_cast<EventBits_t>(0x05)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  EXPECT_EQ(eg.get_bits(), 0x05);
}

TEST_F(SaEventGroupTest, HandleMethod) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::sa::event_group eg;
  EXPECT_EQ(eg.handle(), h);
}

class SaTimerTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x7001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaTimerTest, StartExSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStart(h, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.start_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaTimerTest, StopExSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStop(h, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.stop_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaTimerTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerReset(h, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaTimerTest, PeriodExSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriod(h, 200, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex(200);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaTimerTest, StartExChronoFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStart(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.start_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

TEST_F(SaTimerTest, StopExChronoFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerStop(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.stop_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

TEST_F(SaTimerTest, ResetExChronoFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerReset(h, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.reset_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

TEST_F(SaTimerTest, PeriodExChronoFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xTimerChangePeriod(h, _, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(h, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, [](){});
  auto r = t.period_ex(std::chrono::milliseconds(200), std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

class SaQueueTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x8001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaQueueTest, SendExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_ex(42, 10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueueTest, ReceiveExRefSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  int val;
  auto r = q.receive_ex(val, 10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueueTest, ReceiveExOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.receive_ex(10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueueTest, ReceiveExOptionalFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.receive_ex(10);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueueTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReset(h)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueueTest, OverwriteExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwrite(h, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.overwrite_ex(42);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueueTest, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueueTest, SendBackExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_back_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueueTest, SendFrontExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_front_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueueTest, SendBackExChronoFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_back_ex(42, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueueTest, SendFrontExChronoFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::sa::queue<10, int> q;
  auto r = q.send_front_ex(42, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}
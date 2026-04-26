#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_event_group.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_isr_result.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class IsrBinarySemaphoreTest : public ::testing::Test {
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

TEST_F(IsrBinarySemaphoreTest, GiveIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.give_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrBinarySemaphoreTest, TakeIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.take_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrBinarySemaphoreTest, ReleaseIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto r = sem.release_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

class IsrCountingSemaphoreTest : public ::testing::Test {
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

TEST_F(IsrCountingSemaphoreTest, GiveIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.give_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrCountingSemaphoreTest, TakeIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.take_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrCountingSemaphoreTest, ReleaseIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto r = sem.release_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

class IsrMutexTest : public ::testing::Test {
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

TEST_F(IsrMutexTest, LockIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrMutexTest, LockIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.lock_isr();
  EXPECT_EQ(r.result, pdFALSE);
}

TEST_F(IsrMutexTest, UnlockIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.unlock_isr();
  EXPECT_EQ(r.result, pdTRUE);
}

TEST_F(IsrMutexTest, UnlockIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(h, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto r = m.unlock_isr();
  EXPECT_EQ(r.result, pdFALSE);
}

class IsrQueueTest : public ::testing::Test {
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

TEST_F(IsrQueueTest, SendIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, SendBackIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_back_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, SendFrontIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.send_front_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, ReceiveIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int val;
  auto r = q.receive_isr(val);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, ReceiveIsrOptional) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.receive_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(IsrQueueTest, ReceiveIsrOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.receive_isr();
  EXPECT_FALSE(r.result.has_value());
}

TEST_F(IsrQueueTest, OverwriteIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwriteFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.overwrite_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, PeekIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int val;
  auto r = q.peek_isr(val);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrQueueTest, PeekIsrOptional) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.peek_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(IsrQueueTest, PeekIsrOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto r = q.peek_isr();
  EXPECT_FALSE(r.result.has_value());
}

TEST_F(IsrQueueTest, MessagesWaitingIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, uxQueueMessagesWaitingFromISR(h)).WillOnce(Return(5));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  EXPECT_EQ(q.messages_waiting_isr(), 5);
}

TEST_F(IsrQueueTest, FullIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueFullFromISR(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  EXPECT_EQ(q.full_isr(), pdTRUE);
}

TEST_F(IsrQueueTest, EmptyIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueEmptyFromISR(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  EXPECT_EQ(q.empty_isr(), pdTRUE);
}

class IsrEventGroupTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x4001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(IsrEventGroupTest, SetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(h, 0x01, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  auto r = eg.set_bits_isr(0x01);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(IsrEventGroupTest, GetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupGetBitsFromISR(h)).WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  EXPECT_EQ(eg.get_bits_isr(), 0x03);
}

class IsrStreamBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x5001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(IsrStreamBufferTest, SendIsr) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(h, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  auto r = sb.send_isr("test", 4);
  EXPECT_EQ(r.result, 4u);
}

TEST_F(IsrStreamBufferTest, SendIsrIterator) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(h, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  const char data[] = "test";
  auto r = sb.send_isr(data, data + 4);
  EXPECT_EQ(r.result, 4u);
}

TEST_F(IsrStreamBufferTest, ReceiveIsr) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::stream_buffer<256, freertos::dynamic_stream_buffer_allocator<256>> sb;
  char buf[4];
  auto r = sb.receive_isr(buf, 4);
  EXPECT_EQ(r.result, 4u);
}

class IsrMessageBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x6001);
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(IsrMessageBufferTest, SendIsr) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(h, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  auto r = mb.send_isr("test", 4);
  EXPECT_EQ(r.result, 4u);
}

TEST_F(IsrMessageBufferTest, ReceiveIsr) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(h, _, 4, _)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>> mb;
  char buf[4];
  auto r = mb.receive_isr(buf, 4);
  EXPECT_EQ(r.result, 4u);
}

class IsrResultOperatorsTest : public ::testing::Test {};

TEST_F(IsrResultOperatorsTest, EqualityOperator) {
  freertos::isr_result<BaseType_t> a{pdTRUE, pdFALSE};
  freertos::isr_result<BaseType_t> b{pdTRUE, pdFALSE};
  freertos::isr_result<BaseType_t> c{pdFALSE, pdTRUE};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST_F(IsrResultOperatorsTest, InequalityOperator) {
  freertos::isr_result<BaseType_t> a{pdTRUE, pdFALSE};
  freertos::isr_result<BaseType_t> b{pdFALSE, pdTRUE};
  EXPECT_NE(a, b);
}

TEST_F(IsrResultOperatorsTest, VoidSpecializationEquality) {
  freertos::isr_result<void> a{pdTRUE};
  freertos::isr_result<void> b{pdTRUE};
  freertos::isr_result<void> c{pdFALSE};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST_F(IsrResultOperatorsTest, VoidSpecializationInequality) {
  freertos::isr_result<void> a{pdTRUE};
  freertos::isr_result<void> b{pdFALSE};
  EXPECT_NE(a, b);
}
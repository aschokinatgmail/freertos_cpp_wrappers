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

#include "freertos_task.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
} // namespace

class TaskNotifyTest : public ::testing::Test {
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

TEST_F(TaskNotifyTest, TaskNotifyIsr) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyFromISR(fake_task, 0x01, eSetValueWithoutOverwrite, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_isr(0x01, eSetValueWithoutOverwrite);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TaskNotifyTest, TaskNotifyAndQueryIsr) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  uint32_t prev_value = 0;
  EXPECT_CALL(*mock, xTaskNotifyAndQueryFromISR(fake_task, 0x02, eSetValueWithoutOverwrite, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(0xFF), SetArgPointee<4>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_and_query_isr(0x02, eSetValueWithoutOverwrite, prev_value);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(prev_value, 0xFF);
}

TEST_F(TaskNotifyTest, NotifyStateClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyStateClear(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto rc = t.notify_state_clear();
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(TaskNotifyTest, NotifyValueClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyValueClear(fake_task, 0xFF)).WillOnce(Return(0x42));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto rc = t.notify_value_clear(0xFF);
  EXPECT_EQ(rc, 0x42u);
}

TEST_F(TaskNotifyTest, NotifyTakeChrono) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, _)).WillOnce(Return(5));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto val = t.notify_take(pdTRUE, std::chrono::milliseconds(100));
  EXPECT_EQ(val, 5u);
}

TEST_F(TaskNotifyTest, NotifyWaitChrono) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(0, 0, val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TaskNotifyTest, PeriodicTaskNotifyStateClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyStateClear(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  auto rc = pt.notify_state_clear();
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(TaskNotifyTest, PeriodicTaskNotifyValueClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyValueClear(fake_task, 0x0F)).WillOnce(Return(0x0A));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  auto rc = pt.notify_value_clear(0x0F);
  EXPECT_EQ(rc, 0x0Au);
}

TEST_F(TaskNotifyTest, DynamicTaskAllocatorCreateSuccess) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x5000);
  StrictMock<FreeRTOSMock> *mock2 = new StrictMock<FreeRTOSMock>();
  g_freertos_mock = mock2;
  EXPECT_CALL(*mock2, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h), Return(pdPASS)));

  freertos::dynamic_task_allocator<512> alloc;
  auto result = alloc.create(nullptr, "test", 1, nullptr);
  EXPECT_EQ(result, h);

  delete mock2;
  g_freertos_mock = nullptr;
}

TEST_F(TaskNotifyTest, DynamicTaskAllocatorCreateFailure) {
  StrictMock<FreeRTOSMock> *mock2 = new StrictMock<FreeRTOSMock>();
  g_freertos_mock = mock2;
  EXPECT_CALL(*mock2, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(Return(pdFAIL));

  freertos::dynamic_task_allocator<512> alloc;
  auto result = alloc.create(nullptr, "test", 1, nullptr);
  EXPECT_EQ(result, nullptr);

  delete mock2;
  g_freertos_mock = nullptr;
}

class TimerComplexTest : public ::testing::Test {
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

TEST_F(TimerComplexTest, MoveConstructorNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer src("t", 100, pdTRUE, []() {});
  freertos::da::timer dst(std::move(src));
}

TEST_F(TimerComplexTest, MoveConstructorWithHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, vTimerSetTimerID(fake_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer src("t", 100, pdTRUE, []() {});
  freertos::da::timer dst(std::move(src));
}

TEST_F(TimerComplexTest, MoveAssignmentSrcNull) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
  a = std::move(b);
}

TEST_F(TimerComplexTest, SelfAssignment) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  t = std::move(t);
#pragma GCC diagnostic pop
}

class MoveAssignmentTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4001);
  SemaphoreHandle_t sem2 = reinterpret_cast<SemaphoreHandle_t>(0x4002);
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5001);
  QueueHandle_t q2 = reinterpret_cast<QueueHandle_t>(0x5002);
  EventGroupHandle_t eg1 = reinterpret_cast<EventGroupHandle_t>(0x6001);
  EventGroupHandle_t eg2 = reinterpret_cast<EventGroupHandle_t>(0x6002);
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0x7001);
  StreamBufferHandle_t sb2 = reinterpret_cast<StreamBufferHandle_t>(0x7002);
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0x8001);
  MessageBufferHandle_t mb2 = reinterpret_cast<MessageBufferHandle_t>(0x8002);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MoveAssignmentTest, BinarySemaphoreSwap) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem2));

  freertos::da::binary_semaphore bs1, bs2;
  bs1.swap(bs2);
}

TEST_F(MoveAssignmentTest, CountingSemaphoreSwap) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem2));

  freertos::da::counting_semaphore cs1(5), cs2(5);
  cs1.swap(cs2);
}

TEST_F(MoveAssignmentTest, EventGroupSwap) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(eg1));
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(eg2));
  EXPECT_CALL(*mock, vEventGroupDelete(eg1));
  EXPECT_CALL(*mock, vEventGroupDelete(eg2));

  freertos::da::event_group eg1_obj, eg2_obj;
  eg1_obj.swap(eg2_obj);
}

TEST_F(MoveAssignmentTest, EventGroupMoveAssignOther) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(eg1));
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(eg2));
  EXPECT_CALL(*mock, vEventGroupDelete(eg1));
  EXPECT_CALL(*mock, vEventGroupDelete(eg2));

  freertos::da::event_group a, b;
  a = std::move(b);
}

TEST_F(MoveAssignmentTest, StreamBufferMoveConstructNull) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(nullptr));

  freertos::da::stream_buffer<64> src(1);
  freertos::da::stream_buffer<64> dst(std::move(src));
}

TEST_F(MoveAssignmentTest, StreamBufferSwapBothValid) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb2));

  freertos::da::stream_buffer<64> a(1), b(1);
  a.swap(b);
}

TEST_F(MoveAssignmentTest, StreamBufferMoveAssign) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb2));

  freertos::da::stream_buffer<64> a(1), b(1);
  a = std::move(b);
}

TEST_F(MoveAssignmentTest, MessageBufferMoveConstructNull) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(nullptr));

  freertos::da::message_buffer<64> src;
  freertos::da::message_buffer<64> dst(std::move(src));
}

TEST_F(MoveAssignmentTest, MessageBufferSwapBothValid) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb2));

  freertos::da::message_buffer<64> a, b;
  a.swap(b);
}

TEST_F(MoveAssignmentTest, MessageBufferMoveAssign) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb2));

  freertos::da::message_buffer<64> a, b;
  a = std::move(b);
}

TEST_F(MoveAssignmentTest, QueueOverwrite) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, xQueueOverwrite(q1, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto rc = q.overwrite(val);
  EXPECT_EQ(rc, pdPASS);
}

class NullHandleTest : public ::testing::Test {
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

TEST_F(NullHandleTest, StreamBufferEmptyAndFull) {
  StreamBufferHandle_t sb = reinterpret_cast<StreamBufferHandle_t>(0x7000);
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(sb)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xStreamBufferIsFull(sb)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb));

  freertos::da::stream_buffer<64> s(1);
  EXPECT_EQ(s.empty(), pdTRUE);
  EXPECT_EQ(s.full(), pdFALSE);
}

TEST_F(NullHandleTest, MessageBufferEmptyAndFull) {
  MessageBufferHandle_t mb = reinterpret_cast<MessageBufferHandle_t>(0x8000);
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(mb)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xMessageBufferIsFull(mb)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb));

  freertos::da::message_buffer<64> m;
  EXPECT_EQ(m.empty(), pdTRUE);
  EXPECT_EQ(m.full(), pdFALSE);
}

class QueueIsrTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_queue = reinterpret_cast<QueueHandle_t>(0x5000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueIsrTest, OverwriteIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueOverwriteFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto result = q.overwrite_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueIsrTest, SendFrontIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto result = q.send_front_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueIsrTest, SendBackIsr) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto result = q.send_back_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueIsrTest, ReceiveIsrTypedVariant) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

class EventGroupChronoTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x6000);

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
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x03, pdTRUE, pdTRUE, _))
      .WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits(0x03, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_EQ(result, 0x03);
}

TEST_F(EventGroupChronoTest, SyncChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync(0x01, 0x03, std::chrono::milliseconds(100));
  EXPECT_EQ(result, 0x03);
}

TEST_F(EventGroupChronoTest, SyncExChronoWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, _))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupChronoTest, WaitBitsExChronoTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupChronoTest, WaitBitsExChronoWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, _))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, std::chrono::milliseconds(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupChronoTest, SyncExPartialBits) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(0x01));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

class SemaphoreChronoTest : public ::testing::Test {
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

TEST_F(SemaphoreChronoTest, CountingSemaphoreTakeChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto rc = cs.take(std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreGiveEx) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.give_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreTakeExChronoWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.take_ex(std::chrono::milliseconds(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreTakeExChronoTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.take_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(SemaphoreChronoTest, CountingSemaphoreTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

class QueueExtraTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_queue = reinterpret_cast<QueueHandle_t>(0x5000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueExtraTest, QueueOverwrite) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueOverwrite(fake_queue, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto rc = q.overwrite(val);
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueExtraTest, QueueSendFrontChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto rc = q.send_front(val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueExtraTest, QueueSendBackChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val = 42;
  auto rc = q.send_back(val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

class StreamBufferExtraTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x7000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(StreamBufferExtraTest, ReceiveExIsrSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 3, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(3)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t buf[3];
  auto result = sb.receive_ex_isr(buf, 3);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 3u);
}

TEST_F(StreamBufferExtraTest, ReceiveExIsrFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 3, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(0)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t buf[3];
  auto result = sb.receive_ex_isr(buf, 3);
  EXPECT_FALSE(result.result.has_value());
}

class MessageBufferExtraTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x8000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferExtraTest, SendExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 3, _)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex(data, 3, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3u);
}

TEST_F(MessageBufferExtraTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 3, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex(data, 3, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferExtraTest, SendExTimeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 3, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex(data, 3, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferExtraTest, ReceiveExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 64, _)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex(buf, 64, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3u);
}

TEST_F(MessageBufferExtraTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 64, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex(buf, 64, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferExtraTest, ReceiveExTimeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 64, _)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex(buf, 64, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferExtraTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(fake_mb)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  auto result = mb.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(MessageBufferExtraTest, ResetExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(fake_mb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  auto result = mb.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

class EventGroupIsrTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x6000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupIsrTest, SetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_isr(0x01);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(EventGroupIsrTest, SetBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBits(fake_eg, 0x01)).WillOnce(Return(0x01));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0x01);
}

TEST_F(EventGroupIsrTest, ClearBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupClearBits(fake_eg, 0x01)).WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.clear_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0x03);
}

TEST_F(EventGroupIsrTest, SetBitsExIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(EventGroupIsrTest, GetBits) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupGetBits(fake_eg)).WillOnce(Return(0x05));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto bits = eg.get_bits();
  EXPECT_EQ(bits, 0x05);
}

TEST_F(EventGroupIsrTest, GetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupGetBitsFromISR(fake_eg)).WillOnce(Return(0x07));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto bits = eg.get_bits_isr();
  EXPECT_EQ(bits, 0x07);
}
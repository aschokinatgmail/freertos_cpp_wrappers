#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "mocks/freertos_mocks.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_queue.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_task.hpp"

using namespace std::chrono_literals;

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::StrEq;

using freertos::error;
using freertos::expected;
using freertos::isr_result;
using freertos::unexpected;

using DynQueue = freertos::da::queue<4, int>;
using StaQueue = freertos::sa::queue<4, int>;
using DynTimer = freertos::da::timer;
using StaTimer = freertos::sa::timer;
using DynTask = freertos::da::task<1024>;
using StaTask = freertos::sa::task<1024>;
using DynPeriodicTask = freertos::da::periodic_task<1024>;
using StaPeriodicTask = freertos::sa::periodic_task<1024>;

class CoverageQueueTimerTaskTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_queue = reinterpret_cast<QueueHandle_t>(0x1234);
    mock_queue2 = reinterpret_cast<QueueHandle_t>(0x5678);
    mock_timer = reinterpret_cast<TimerHandle_t>(0x2000);
    mock_timer2 = reinterpret_cast<TimerHandle_t>(0x2001);
    mock_task = reinterpret_cast<TaskHandle_t>(0x3000);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t mock_queue;
  QueueHandle_t mock_queue2;
  TimerHandle_t mock_timer;
  TimerHandle_t mock_timer2;
  TaskHandle_t mock_task;
};

// =============================================================================
// QUEUE: Allocator swap
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaticQueueAllocatorSwap) {
  freertos::static_queue_allocator<4, int> a1;
  freertos::static_queue_allocator<4, int> a2;
  a1.swap(a2);
}

// =============================================================================
// QUEUE: Move constructor / move assignment / swap
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueMoveConstructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  StaQueue q1;
  StaQueue q2 = std::move(q1);
}

TEST_F(CoverageQueueTimerTaskTest, QueueMoveAssignment) {
  InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue2));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue2));

  StaQueue q1;
  StaQueue q2;
  q2 = std::move(q1);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSwap) {
  InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue2));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue2));

  StaQueue q1;
  StaQueue q2;
  q1.swap(q2);
}

TEST_F(CoverageQueueTimerTaskTest, QueueStdSwap) {
  InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue2));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue2));

  StaQueue q1;
  StaQueue q2;
  using std::swap;
  swap(q1, q2);
}

TEST_F(CoverageQueueTimerTaskTest, DynQueueMoveConstructor) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q1;
  DynQueue q2 = std::move(q1);
}

TEST_F(CoverageQueueTimerTaskTest, DynQueueMoveAssignment) {
  InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue2));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue2));

  DynQueue q1;
  DynQueue q2;
  q2 = std::move(q1);
}

// =============================================================================
// QUEUE: Constructor with allocator args + registry name
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), NotNull(), NotNull()))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue, StrEq("myq")));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return("myq"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::static_queue_allocator<4, int> alloc;
  freertos::queue<4, int, freertos::static_queue_allocator<4, int>> q("myq", std::move(alloc));
}

// =============================================================================
// QUEUE: Destructor with registered name (unregister path)
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueDestructorWithRegisteredName) {
  const char *name = "RegQ";
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue, StrEq(name)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillRepeatedly(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q(name);
}

// =============================================================================
// QUEUE: receive_ex failure paths (would_block, timeout)
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueSendExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_ex(val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_ex(val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_ex(val, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendBackExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_back_ex(val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendBackExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_back_ex(val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendFrontExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_front_ex(val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendFrontExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_front_ex(val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendFrontExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_front_ex(val, std::chrono::milliseconds(200));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// QUEUE: send_ex_isr failure
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueSendExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_ex_isr(val);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendBackExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_back_ex_isr(val);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, QueueSendFrontExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(mock_queue, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 42;
  auto result = q.send_front_ex_isr(val);
  EXPECT_FALSE(result.result.has_value());
}

// =============================================================================
// QUEUE: receive_ex failure paths
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.receive_ex(val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.receive_ex(val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.receive_ex(val, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExValueWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.receive_ex(TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExValueTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.receive_ex(TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveExValueChrono) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.receive_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// QUEUE: reset_ex, overwrite_ex failure
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueResetExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReset(mock_queue)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::invalid_handle);
}

TEST_F(CoverageQueueTimerTaskTest, QueueResetExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReset(mock_queue)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, QueueOverwriteExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(1, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueOverwrite(mock_queue, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<1, int> q;
  int val = 42;
  auto result = q.overwrite_ex(val);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

// =============================================================================
// QUEUE: peek_ex failure paths
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueuePeekExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.peek_ex(val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

TEST_F(CoverageQueueTimerTaskTest, QueuePeekExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.peek_ex(val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, QueuePeekExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  int val = 0;
  auto result = q.peek_ex(val, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// QUEUE: receive with chrono optional return
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveChronoOptional) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.receive(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, QueueReceiveChronoOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.receive(std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// QUEUE: peek with chrono optional return
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, QueuePeekChronoOptional) {
  EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int))).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  DynQueue q;
  auto result = q.peek(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

// =============================================================================
// SW_TIMER: Constructor with allocator args
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  freertos::static_sw_timer_allocator alloc;
  StaTimer t("test", 100, pdTRUE, nullptr, std::move(alloc));
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerChronoConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  freertos::static_sw_timer_allocator alloc;
  StaTimer t("test", std::chrono::milliseconds(100), pdTRUE, nullptr, std::move(alloc));
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  freertos::dynamic_sw_timer_allocator alloc;
  DynTimer t("test", 100, pdTRUE, nullptr, std::move(alloc));
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerChronoConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  freertos::dynamic_sw_timer_allocator alloc;
  DynTimer t("test", std::chrono::milliseconds(100), pdTRUE, nullptr, std::move(alloc));
}

// =============================================================================
// SW_TIMER: Destructor with null + non-null handle
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerDestructorNonNull) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  { StaTimer t("test", 100, pdTRUE, nullptr); }
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerDestructorNull) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  { StaTimer t("test", 100, pdTRUE, nullptr); }
}

// =============================================================================
// SW_TIMER: Move assignment - source was started
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerMoveAssignmentWasStarted) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));

  StaTimer src("src", 100, pdTRUE, nullptr);
  src.start(0);

  TimerHandle_t dest_handle = reinterpret_cast<TimerHandle_t>(0x5000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(dest_handle));

  StaTimer dest("dest", 200, pdFALSE, nullptr);

  EXPECT_CALL(*mock, xTimerStop(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(mock_timer)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(mock_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(mock_timer)).WillOnce(Return(pdTRUE));

  TimerHandle_t new_handle = reinterpret_cast<TimerHandle_t>(0x6000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(new_handle));

  EXPECT_CALL(*mock, xTimerDelete(dest_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  EXPECT_CALL(*mock, xTimerStart(new_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));

  EXPECT_CALL(*mock, xTimerDelete(new_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));

  dest = std::move(src);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerMoveAssignmentWasStartedButStartFails) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));

  StaTimer src("src", 100, pdTRUE, nullptr);
  src.start(0);

  TimerHandle_t dest_handle = reinterpret_cast<TimerHandle_t>(0x5000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(dest_handle));

  StaTimer dest("dest", 200, pdFALSE, nullptr);

  EXPECT_CALL(*mock, xTimerStop(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(mock_timer)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(mock_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(mock_timer)).WillOnce(Return(pdTRUE));

  TimerHandle_t new_handle = reinterpret_cast<TimerHandle_t>(0x6000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(new_handle));

  EXPECT_CALL(*mock, xTimerDelete(dest_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  EXPECT_CALL(*mock, xTimerStart(new_handle, portMAX_DELAY)).WillOnce(Return(pdFAIL));

  EXPECT_CALL(*mock, xTimerDelete(new_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));

  dest = std::move(src);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerMoveAssignmentRecreationFails) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));

  StaTimer src("src", 100, pdTRUE, nullptr);
  src.start(0);

  TimerHandle_t dest_handle = reinterpret_cast<TimerHandle_t>(0x5000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(dest_handle));

  StaTimer dest("dest", 200, pdFALSE, nullptr);

  EXPECT_CALL(*mock, xTimerStop(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(mock_timer)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(mock_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(mock_timer)).WillOnce(Return(pdTRUE));

  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  EXPECT_CALL(*mock, xTimerDelete(dest_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  dest = std::move(src);
}

// =============================================================================
// SW_TIMER: Move assignment - source timer is null
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerMoveAssignmentSrcNull) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer src("src", 100, pdTRUE, nullptr);

  TimerHandle_t dest_handle = reinterpret_cast<TimerHandle_t>(0x5000);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(dest_handle));

  StaTimer dest("dest", 200, pdFALSE, nullptr);

  EXPECT_CALL(*mock, xTimerDelete(dest_handle, portMAX_DELAY)).WillOnce(Return(pdPASS));

  dest = std::move(src);
}

// =============================================================================
// SW_TIMER: swap
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerSwap) {
  InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer2));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer2, _));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer2, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t1("t1", 100, pdTRUE, nullptr);
  StaTimer t2("t2", 200, pdFALSE, nullptr);
  t1.swap(t2);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerStdSwap) {
  InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer2));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer2, _));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer2, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t1("t1", 100, pdTRUE, nullptr);
  StaTimer t2("t2", 200, pdFALSE, nullptr);
  using std::swap;
  swap(t1, t2);
}

// =============================================================================
// SW_TIMER: _ex failure paths
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerStartExFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.start_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timer_queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerStartExChrono) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.start_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerStopExFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStop(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.stop_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timer_queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerReset(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.reset_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timer_queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerPeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, 200, _)).WillOnce(Return(pdFAIL));
  auto result = t.period_ex(200);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timer_queue_full);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerPeriodExChrono) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, _, _)).WillOnce(Return(pdFAIL));
  auto result = t.period_ex(std::chrono::milliseconds(200), std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// SW_TIMER: _ex_isr failure paths
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerStartExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStartFromISR(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.start_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::invalid_handle);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerStopExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStopFromISR(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.stop_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::invalid_handle);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerResetExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerResetFromISR(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::invalid_handle);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerPeriodExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, 200, _)).WillOnce(Return(pdFAIL));
  auto result = t.period_ex_isr(200);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::invalid_handle);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerPeriodExIsrChrono) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, _, _)).WillOnce(Return(pdFAIL));
  auto result = t.period_ex_isr(std::chrono::milliseconds(200));
  EXPECT_FALSE(result.result.has_value());
}

// =============================================================================
// SW_TIMER: null timer handle paths for accessors
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandlePeriodTicks) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.period_ticks(), 0);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleRemaining) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.remaining_ticks(), 0);
  EXPECT_EQ(t.remaining_time().count(), 0);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleName) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.name(), nullptr);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleReloadMode) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  t.reload_mode(pdFALSE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleStartEx) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  auto result = t.start_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleStopEx) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  auto result = t.stop_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandleResetEx) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  auto result = t.reset_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerNullHandlePeriodEx) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));

  StaTimer t("test", 100, pdTRUE, nullptr);
  auto result = t.period_ex(200);
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// SW_TIMER: start with null timer
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, DynTimerNullHandleStartFails) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.start(), pdFAIL);
}

// =============================================================================
// SW_TIMER: _ex_isr success paths
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerStartExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStartFromISR(mock_timer, _)).WillOnce(Return(pdPASS));
  auto result = t.start_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerStopExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStopFromISR(mock_timer, _)).WillOnce(Return(pdPASS));
  auto result = t.stop_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerResetExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerResetFromISR(mock_timer, _)).WillOnce(Return(pdPASS));
  auto result = t.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerPeriodExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, 200, _)).WillOnce(Return(pdPASS));
  auto result = t.period_ex_isr(200);
  EXPECT_TRUE(result.result.has_value());
}

// =============================================================================
// SW_TIMER: set_reload_mode and is_active/get_period accessors
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerSetReloadMode) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, vTimerSetReloadMode(mock_timer, pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  t.reload_mode(pdFALSE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerGetReloadMode) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(mock_timer)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.reload_mode(), pdFALSE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerIsActive) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerIsTimerActive(mock_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.running(), pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTimerGetPeriod) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerGetPeriod(mock_timer)).Times(2).WillRepeatedly(Return(500));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.period_ticks(), 500);
  EXPECT_EQ(t.period().count(), 500);
}

// =============================================================================
// SW_TIMER: Move constructor
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTimerMoveConstructor) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  StaTimer src("src", 100, pdTRUE, nullptr);
  StaTimer dest = std::move(src);
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerMoveConstructor) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  DynTimer src("src", 100, pdTRUE, nullptr);
  DynTimer dest = std::move(src);
}

// =============================================================================
// TASK: Static task allocator swap + default constructor
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaticTaskAllocatorSwap) {
  freertos::static_task_allocator<1024> a1;
  freertos::static_task_allocator<1024> a2;
  a1.swap(a2);
}

TEST_F(CoverageQueueTimerTaskTest, StaticTaskAllocatorDefaultAndMoveCtor) {
  freertos::static_task_allocator<1024> a1;
  freertos::static_task_allocator<1024> a2 = std::move(a1);
  (void)a2;
}

// =============================================================================
// TASK: move constructor
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskMoveConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t1("task", 2, []() {});
  StaTask t2 = std::move(t1);
  EXPECT_EQ(t2.handle(), mock_task);
}

TEST_F(CoverageQueueTimerTaskTest, DynTaskMoveConstructor) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  DynTask t1("task", 2, []() {});
  DynTask t2 = std::move(t1);
  EXPECT_EQ(t2.handle(), mock_task);
}

// =============================================================================
// TASK: move assignment
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskMoveAssignment) {
  ::testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3001);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  StaTask t1("task", 2, []() {});
  StaTask t2("task2", 1, []() {});
  t2 = std::move(t1);
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
}

// =============================================================================
// TASK: swap
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskSwap) {
  ::testing::InSequence seq;
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3002);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  StaTask t1("t1", 2, []() {});
  StaTask t2("t2", 1, []() {});
  t1.swap(t2);
  EXPECT_EQ(t1.handle(), h2);
  EXPECT_EQ(t2.handle(), h1);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskStdSwap) {
  ::testing::InSequence seq;
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3002);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  StaTask t1("t1", 2, []() {});
  StaTask t2("t2", 1, []() {});
  using std::swap;
  swap(t1, t2);
}

// =============================================================================
// TASK: handle(), state(), is_running, is_suspended, is_alive
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskHandle) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.handle(), mock_task);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskState) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eRunning));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.state(), eRunning);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIsRunningDefaultPath) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eInvalid));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_FALSE(t.is_running());
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIsSuspended) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_TRUE(t.is_suspended());
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIsAliveDeletedState) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_FALSE(t.is_alive());
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIsAliveInvalidState) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eInvalid));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_FALSE(t.is_alive());
}

// =============================================================================
// TASK: notify_wait_ex failure
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyWaitExWouldBlock) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t notif_val = 0;
  auto result = t.notify_wait_ex(0, 0, notif_val, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyWaitExTimeout) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t notif_val = 0;
  auto result = t.notify_wait_ex(0, 0, notif_val, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyWaitExChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(_, _, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t notif_val = 0;
  auto result = t.notify_wait_ex(0, 0, notif_val, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyWaitExSuccess) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 100))
      .WillOnce(DoAll(SetArgPointee<2>(42), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t notif_val = 0;
  auto result = t.notify_wait_ex(0, 0, notif_val, TickType_t(100));
  EXPECT_TRUE(result.has_value());
}

// =============================================================================
// TASK: delay / delay_until with chrono templates
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, TaskDelayChrono) {
  EXPECT_CALL(*mock, vTaskDelay(pdMS_TO_TICKS(500)));
  freertos::delay(std::chrono::milliseconds(500));
}

TEST_F(CoverageQueueTimerTaskTest, TaskDelayUntilChrono) {
  TickType_t wake = 100;
  EXPECT_CALL(*mock, vTaskDelayUntil(&wake, pdMS_TO_TICKS(250)));
  freertos::delay_until(wake, std::chrono::milliseconds(250));
}

// =============================================================================
// TASK: constructor with start_suspended=false
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskConstructorNotSuspended) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {}, false);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskConstructorSuspended) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {}, true);
}

// =============================================================================
// TASK: null handle destructor
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskDestructorNullHandle) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(nullptr));

  { StaTask t("task", 2, []() {}); }
}

// =============================================================================
// TASK: notify_take_ex/notify_wait_ex with null handle
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyTakeChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100))).WillOnce(Return(1));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t result = t.notify_take(pdTRUE, std::chrono::milliseconds(100));
  EXPECT_EQ(result, 1);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskNotifyWaitChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, pdMS_TO_TICKS(100)))
      .WillOnce(DoAll(SetArgPointee<2>(0xABCD), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t val = 0;
  BaseType_t result = t.notify_wait(0, 0, val, std::chrono::milliseconds(100));
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(val, 0xABCD);
}

// =============================================================================
// TASK: periodic_task move + constructors with chrono period
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskMoveConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms);
  StaPeriodicTask pt2 = std::move(pt);
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskMoveAssignment) {
  ::testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x4000)));
  EXPECT_CALL(*mock, xTaskAbortDelay(reinterpret_cast<TaskHandle_t>(0x4000))).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(reinterpret_cast<TaskHandle_t>(0x4000))).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(reinterpret_cast<TaskHandle_t>(0x4000)));

  StaPeriodicTask pt1("pt1", 2, []() {}, []() {}, []() {}, 100ms);
  StaPeriodicTask pt2("pt2", 1, []() {}, []() {}, []() {}, 50ms);
  pt2 = std::move(pt1);
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskChronoConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, std::chrono::seconds(1));
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNoPeriodConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {});
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNoPeriodStringConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  std::string name = "pt";
  StaPeriodicTask pt(name, 2, []() {}, []() {}, []() {});
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskIsRunningStateCheck) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms);

  EXPECT_CALL(*mock, eTaskGetState(mock_task)).WillOnce(Return(eDeleted));
  EXPECT_FALSE(pt.is_running());
}

// =============================================================================
// TASK: periodic_task swap
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskSwap) {
  ::testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x4000);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(h2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  StaPeriodicTask pt1("pt1", 2, []() {}, []() {}, []() {}, 100ms);
  StaPeriodicTask pt2("pt2", 1, []() {}, []() {}, []() {}, 50ms);
  pt1.swap(pt2);
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskStdSwap) {
  ::testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x4000);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(h2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  StaPeriodicTask pt1("pt1", 2, []() {}, []() {}, []() {}, 100ms);
  StaPeriodicTask pt2("pt2", 1, []() {}, []() {}, []() {}, 50ms);
  using std::swap;
  swap(pt1, pt2);
}

// =============================================================================
// TASK: periodic_task with chrono constructor + allocator args
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskChronoConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  freertos::static_task_allocator<1024> alloc;
  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms, true, std::move(alloc));
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNoPeriodConstructorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  freertos::static_task_allocator<1024> alloc;
  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, true, std::move(alloc));
}

// =============================================================================
// TASK: string name constructors
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaTaskStringNameConstructor) {
  std::string name = "StrTask";
  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("StrTask"), _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t(name, 2, []() {});
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskStringNameChronoConstructor) {
  std::string name = "StrPT";
  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("StrPT"), _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt(name, 2, []() {}, []() {}, []() {}, 100ms);
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskStringNameNoPeriodConstructor) {
  std::string name = "StrPT2";
  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("StrPT2"), _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt(name, 2, []() {}, []() {}, []() {});
}

// =============================================================================
// TASK: indexed notifications (if enabled)
// =============================================================================

#if configTASK_NOTIFICATION_ARRAY_ENTRIES > 1

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyGive) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(mock_task, 1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify_give(1), pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyTake) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdTRUE, 100)).WillOnce(Return(5));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify_take(1, pdTRUE, 100), 5u);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyTakeChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdTRUE, pdMS_TO_TICKS(100))).WillOnce(Return(3));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify_take(1, pdTRUE, std::chrono::milliseconds(100)), 3u);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotify) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyIndexed(mock_task, 1, 0x42, eSetBits)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify(1, 0x42, eSetBits), pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyAndQuery) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexed(mock_task, 1, 0x42, eIncrement, _))
      .WillOnce(DoAll(SetArgPointee<4>(0x11), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t prev = 0;
  EXPECT_EQ(t.notify_and_query(1, 0x42, eIncrement, prev), pdTRUE);
  EXPECT_EQ(prev, 0x11u);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyIsr) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(mock_task, 1, 0x42, eSetBits, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  auto res = t.notify_isr(1, 0x42, eSetBits);
  EXPECT_EQ(res.result, pdTRUE);
  EXPECT_EQ(res.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyAndQueryIsr) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexedFromISR(mock_task, 1, 0x42, eIncrement, _, _))
      .WillOnce(DoAll(SetArgPointee<4>(0x11), SetArgPointee<5>(pdFALSE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t prev = 0;
  auto res = t.notify_and_query_isr(1, 0x42, eIncrement, prev);
  EXPECT_EQ(res.result, pdTRUE);
  EXPECT_EQ(prev, 0x11u);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyWait) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, 100))
      .WillOnce(DoAll(SetArgPointee<3>(0xAB), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t val = 0;
  EXPECT_EQ(t.notify_wait(1, 0, 0, val, 100), pdTRUE);
  EXPECT_EQ(val, 0xABu);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyWaitChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(0xCD), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  uint32_t val = 0;
  EXPECT_EQ(t.notify_wait(1, 0, 0, val, std::chrono::milliseconds(100)), pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyStateClear) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyStateClearIndexed(mock_task, 1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify_state_clear(1), pdTRUE);
}

TEST_F(CoverageQueueTimerTaskTest, StaTaskIndexedNotifyValueClear) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, ulTaskNotifyValueClearIndexed(mock_task, 1, 0xFF)).WillOnce(Return(0x00FF));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaTask t("task", 2, []() {});
  EXPECT_EQ(t.notify_value_clear(1, 0xFF), 0x00FFu);
}

#endif

// =============================================================================
// TASK: notify_wait_ex on periodic task
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNotifyWaitExSuccess) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(42), Return(pdTRUE)));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms);
  uint32_t val = 0;
  auto result = pt.notify_wait_ex(0, 0, val, 100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNotifyWaitExFailure) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(_, _, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms);
  uint32_t val = 0;
  auto result = pt.notify_wait_ex(0, 0, val, 100);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, StaPeriodicTaskNotifyWaitExChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(mock_task));
  EXPECT_CALL(*mock, xTaskNotifyWait(_, _, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  StaPeriodicTask pt("pt", 2, []() {}, []() {}, []() {}, 100ms);
  uint32_t val = 0;
  auto result = pt.notify_wait_ex(0, 0, val, std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
}

// =============================================================================
// Dynamic allocator timer variants
// =============================================================================

TEST_F(CoverageQueueTimerTaskTest, DynTimerStartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.start_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerStopExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerStop(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.stop_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerReset(mock_timer, _)).WillOnce(Return(pdFAIL));
  auto result = t.reset_ex(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerPeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, 200, _)).WillOnce(Return(pdFAIL));
  auto result = t.period_ex(200);
  EXPECT_FALSE(result.has_value());
}

TEST_F(CoverageQueueTimerTaskTest, DynTimerNullHandleAccessors) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  DynTimer t("test", 100, pdTRUE, nullptr);
  EXPECT_EQ(t.start(), pdFAIL);
  EXPECT_EQ(t.stop(), pdFAIL);
  EXPECT_EQ(t.reset(), pdFAIL);
  EXPECT_EQ(t.period(200), pdFAIL);
  EXPECT_EQ(t.start_isr().result, pdFAIL);
  EXPECT_EQ(t.stop_isr().result, pdFAIL);
  EXPECT_EQ(t.reset_isr().result, pdFAIL);
  EXPECT_EQ(t.period_isr(200).result, pdFAIL);
  EXPECT_EQ(t.running(), pdFALSE);
  EXPECT_EQ(t.period_ticks(), 0);
  EXPECT_EQ(t.remaining_ticks(), 0);
  EXPECT_EQ(t.name(), nullptr);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_task.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_queue.hpp"
#include "freertos_event_group.hpp"
#include "freertos_external_allocator.hpp"
#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::SaveArg;
using ::testing::Invoke;
using ::testing::InSequence;
using ::testing::Expectation;

// =============================================================================
// Task: task_exec, operator=, suspend, is_running, is_suspended, is_alive
// =============================================================================

class TaskCoverage2Test : public ::testing::Test {
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

TEST_F(TaskCoverage2Test, DynamicAllocator1024TaskExec) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::dynamic_task_allocator<1024>> t("t", 1, []() {}, false);
  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);
}

TEST_F(TaskCoverage2Test, DynamicAllocator2048TaskExec) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::dynamic_task_allocator<2048>> t("t", 1, []() {}, false);
  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);
}

TEST_F(TaskCoverage2Test, DynamicAllocator512TaskExec) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::dynamic_task_allocator<512>> t("t", 1, []() {}, false);
  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);
}

TEST_F(TaskCoverage2Test, StaticAllocator0TaskExec) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       Return(fake_task)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<0>> t("t", 1, []() {}, false);
  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);
}

TEST_F(TaskCoverage2Test, DynamicAllocator512TaskExecSuspended) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskSuspend(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::dynamic_task_allocator<512>> t("t", 1, []() {}, true);
  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);
}

TEST_F(TaskCoverage2Test, DynamicAllocator512MoveAssignment) {
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h1), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h2), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  freertos::task<freertos::dynamic_task_allocator<512>> t1("t1", 1, []() {}, false);
  freertos::task<freertos::dynamic_task_allocator<512>> t2("t2", 1, []() {}, false);
  t1 = std::move(t2);
}

TEST_F(TaskCoverage2Test, DynamicAllocator1024Suspend) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskSuspend(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::dynamic_task_allocator<1024>> t("t", 1, []() {}, false);
  t.suspend();
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsRunning) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eRunning));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_TRUE(t.is_running());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsNotRunning) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_FALSE(t.is_running());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsSuspended) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_TRUE(t.is_suspended());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsNotSuspended) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eReady));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_FALSE(t.is_suspended());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsAlive) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eReady));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_TRUE(t.is_alive());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsNotAliveDeleted) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_FALSE(t.is_alive());
}

TEST_F(TaskCoverage2Test, StaticAllocator512IsNotAliveInvalid) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eInvalid));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::task<freertos::static_task_allocator<512>> t("t", 1, []() {}, false);
  EXPECT_FALSE(t.is_alive());
}

// =============================================================================
// Periodic task: constructors and run with various allocators
// Using NiceMock because periodic_task::run() calls is_alive() and notify_wait
// in a loop; we set up ON_CALL defaults and explicit EXPECT_CALL for order
// =============================================================================

class PeriodicTaskCoverage2Test : public ::testing::Test {
protected:
  NiceMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3000);
  int eTaskGetState_call_count = 0;

  void SetUp() override {
    mock = new NiceMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(PeriodicTaskCoverage2Test, DynamicAllocator512RunOnce) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;

  bool on_start_called = false;
  bool on_stop_called = false;
  bool periodic_called = false;

  eTaskGetState_call_count = 0;
  ON_CALL(*mock, eTaskGetState(fake_task))
      .WillByDefault(testing::InvokeWithoutArgs([this]() -> eTaskState {
        eTaskGetState_call_count++;
        return (eTaskGetState_call_count <= 1) ? eReady : eDeleted;
      }));
  ON_CALL(*mock, xTaskNotifyWait(_, _, _, _)).WillByDefault(Return(pdTRUE));

  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      [&periodic_called]() { periodic_called = true; },
      std::chrono::milliseconds(100), false);

  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(periodic_called);
  EXPECT_TRUE(on_stop_called);
}

TEST_F(PeriodicTaskCoverage2Test, StaticAllocator1024RunOnce) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;

  bool on_start_called = false;
  bool on_stop_called = false;
  bool periodic_called = false;

  eTaskGetState_call_count = 0;
  ON_CALL(*mock, eTaskGetState(fake_task))
      .WillByDefault(testing::InvokeWithoutArgs([this]() -> eTaskState {
        eTaskGetState_call_count++;
        return (eTaskGetState_call_count <= 1) ? eReady : eDeleted;
      }));
  ON_CALL(*mock, xTaskNotifyWait(_, _, _, _)).WillByDefault(Return(pdTRUE));

  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       Return(fake_task)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      [&periodic_called]() { periodic_called = true; },
      std::chrono::milliseconds(100), false);

  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(periodic_called);
  EXPECT_TRUE(on_stop_called);
}

TEST_F(PeriodicTaskCoverage2Test, DynamicAllocator512ChronoWithAllocatorArg) {
  ON_CALL(*mock, eTaskGetState(_)).WillByDefault(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::dynamic_task_allocator<512> alloc;
  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::milliseconds(100), false, std::move(alloc));
}

TEST_F(PeriodicTaskCoverage2Test, StaticAllocator1024ChronoWithAllocatorArg) {
  ON_CALL(*mock, eTaskGetState(_)).WillByDefault(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::static_task_allocator<1024> alloc;
  freertos::periodic_task<freertos::static_task_allocator<1024>> pt(
      "pt", 1, []() {}, []() {}, []() {},
      std::chrono::milliseconds(100), false, std::move(alloc));
}

TEST_F(PeriodicTaskCoverage2Test, DynamicAllocator512NoPeriodConstructor) {
  TaskFunction_t captured_func = nullptr;
  void *captured_ctx = nullptr;

  bool on_start_called = false;
  bool on_stop_called = false;
  bool periodic_called = false;

  eTaskGetState_call_count = 0;
  ON_CALL(*mock, eTaskGetState(fake_task))
      .WillByDefault(testing::InvokeWithoutArgs([this]() -> eTaskState {
        eTaskGetState_call_count++;
        return (eTaskGetState_call_count <= 1) ? eReady : eDeleted;
      }));

  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<0>(&captured_func), SaveArg<3>(&captured_ctx),
                       SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1,
      [&on_start_called]() { on_start_called = true; },
      [&on_stop_called]() { on_stop_called = true; },
      [&periodic_called]() { periodic_called = true; },
      false);

  ASSERT_NE(captured_func, nullptr);
  ASSERT_NE(captured_ctx, nullptr);
  captured_func(captured_ctx);

  EXPECT_TRUE(on_start_called);
  EXPECT_TRUE(periodic_called);
  EXPECT_TRUE(on_stop_called);
}

TEST_F(PeriodicTaskCoverage2Test, DynamicAllocator512NoPeriodWithAllocArg) {
  ON_CALL(*mock, eTaskGetState(_)).WillByDefault(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::dynamic_task_allocator<512> alloc;
  freertos::periodic_task<freertos::dynamic_task_allocator<512>> pt(
      "pt", 1, []() {}, []() {}, []() {}, false, std::move(alloc));
}

// =============================================================================
// Timer: TimerArgAllocator and external_sw_timer_allocator callback_wrapper,
//        SFINAE constructors, destructor
// =============================================================================

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

namespace {
struct TestExternalRegion {
  static void *allocate(size_t) { return &s_timer_buf; }
  static void deallocate(void *) {}
  static StaticTimer_t s_timer_buf;
};
StaticTimer_t TestExternalRegion::s_timer_buf{};
} // namespace

class TimerCoverage2Test : public ::testing::Test {
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

TEST_F(TimerCoverage2Test, TimerArgAllocatorCallbackWrapper) {
  TimerCallbackFunction_t captured_cb = nullptr;
  auto *timer_obj_ptr = static_cast<void *>(nullptr);

  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(DoAll(SaveArg<4>(&captured_cb), Return(fake_timer)));
  EXPECT_CALL(*mock, pvTimerGetTimerID(fake_timer))
      .WillOnce(Invoke([&](TimerHandle_t) -> void * {
        return timer_obj_ptr;
      }));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  bool callback_called = false;
  TimerAllocatorArg arg{1};
  auto t = std::make_unique<freertos::timer<TimerArgAllocator>>("t", 100, pdTRUE,
      [&callback_called]() { callback_called = true; }, arg);
  timer_obj_ptr = reinterpret_cast<void *>(t.get());

  ASSERT_NE(captured_cb, nullptr);
  captured_cb(fake_timer);
  EXPECT_TRUE(callback_called);
  t.reset();
}

TEST_F(TimerCoverage2Test, ExternalSwTimerAllocatorCallbackWrapper) {
  TimerCallbackFunction_t captured_cb = nullptr;
  auto *timer_obj_ptr = static_cast<void *>(nullptr);

  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(DoAll(SaveArg<4>(&captured_cb), Return(fake_timer)));
  EXPECT_CALL(*mock, pvTimerGetTimerID(fake_timer))
      .WillOnce(Invoke([&](TimerHandle_t) -> void * {
        return timer_obj_ptr;
      }));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  bool callback_called = false;
  TestExternalRegion region;
  freertos::external_sw_timer_allocator<TestExternalRegion> alloc(region);
  auto t = std::make_unique<freertos::timer<freertos::external_sw_timer_allocator<TestExternalRegion>>>(
      "t", 100, pdTRUE,
      [&callback_called]() { callback_called = true; },
      std::move(alloc));
  timer_obj_ptr = reinterpret_cast<void *>(t.get());

  ASSERT_NE(captured_cb, nullptr);
  captured_cb(fake_timer);
  EXPECT_TRUE(callback_called);
  t.reset();
}

TEST_F(TimerCoverage2Test, TimerArgAllocatorSFNAEConstructor) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  TimerAllocatorArg arg{2};
  freertos::timer<TimerArgAllocator> t("t", 100, pdTRUE, []() {}, arg);
}

TEST_F(TimerCoverage2Test, TimerArgAllocatorChronoSFNAEConstructor) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  TimerAllocatorArg arg{3};
  freertos::timer<TimerArgAllocator> t(
      "t", std::chrono::milliseconds(100), pdTRUE, []() {}, arg);
}

TEST_F(TimerCoverage2Test, TimerArgAllocatorDestructor) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  TimerAllocatorArg arg{4};
  {
    freertos::timer<TimerArgAllocator> t("t", 100, pdTRUE, []() {}, arg);
  }
}

// =============================================================================
// Queue: QueueArgAllocator constructor/destructor, chrono receive/peek on
//        dynamic_queue_allocator
// =============================================================================

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

class QueueCoverage2Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x4000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueCoverage2Test, QueueArgAllocatorConstructorNoName) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  QueueAllocatorArg arg{1};
  freertos::queue<10, int, QueueArgAllocator> q(nullptr, arg);
}

TEST_F(QueueCoverage2Test, QueueArgAllocatorConstructorWithName) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, _));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return("qname"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  QueueAllocatorArg arg{2};
  freertos::queue<10, int, QueueArgAllocator> q("qname", arg);
}

TEST_F(QueueCoverage2Test, QueueArgAllocatorDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  QueueAllocatorArg arg{3};
  {
    freertos::queue<10, int, QueueArgAllocator> q(nullptr, arg);
  }
}

TEST_F(QueueCoverage2Test, DynamicQueueAllocatorReceiveChronoRef) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int item = 0;
  auto rc = q.receive(item, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueCoverage2Test, DynamicQueueAllocatorReceiveChronoOptional) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto result = q.receive(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverage2Test, DynamicQueueAllocatorPeekChronoRef) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  int item = 0;
  auto rc = q.peek(item, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueCoverage2Test, DynamicQueueAllocatorPeekChronoOptional) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::queue<10, int, freertos::dynamic_queue_allocator<10, int>> q;
  auto result = q.peek(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

// =============================================================================
// Event group: EventGroupArgAllocator constructor/destructor
// =============================================================================

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

class EventGroupCoverage2Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x2000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupCoverage2Test, EventGroupArgAllocatorConstructor) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  EventGroupAllocatorArg arg{42};
  freertos::event_group<EventGroupArgAllocator> eg(arg);
}

TEST_F(EventGroupCoverage2Test, EventGroupArgAllocatorDestructor) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  EventGroupAllocatorArg arg{43};
  {
    freertos::event_group<EventGroupArgAllocator> eg(arg);
  }
}

TEST_F(EventGroupCoverage2Test, EventGroupArgAllocatorDestructorNullHandle) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(nullptr));

  EventGroupAllocatorArg arg{44};
  {
    freertos::event_group<EventGroupArgAllocator> eg(arg);
  }
}
#include <chrono>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "FreeRTOS.h"
#include "../include/freertos_external_allocator.hpp"
#include "../include/freertos_isr_result.hpp"
#include "../include/freertos_sw_timer.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

class TimerCovTest : public Test {
protected:
  void SetUp() override {
    mock = std::make_unique<StrictMock<FreeRTOSMock>>();
    g_freertos_mock = mock.get();
    mock_timer_handle = reinterpret_cast<TimerHandle_t>(0x12345678);
  }

  void TearDown() override {
    g_freertos_mock = nullptr;
    mock.reset();
  }

  std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
  TimerHandle_t mock_timer_handle;
};

// =============================================================================
// Destructor branches: L222 (m_timer != null), L223 (calling xTimerDelete)
// =============================================================================

TEST_F(TimerCovTest, DynamicTimerDestructorNonNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("dtor_test", 100, pdTRUE, std::move(cb));
}

TEST_F(TimerCovTest, DynamicTimerDestructorNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("dtor_null", 100, pdTRUE, std::move(cb));
}

// =============================================================================
// Start: L308 (m_timer null -> return pdFAIL), L310 (xTimerStart success/fail)
// =============================================================================

TEST_F(TimerCovTest, StartSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("start_ok", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.start(), pdPASS);
}

TEST_F(TimerCovTest, StartFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("start_fail", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.start(), pdFAIL);
}

TEST_F(TimerCovTest, StartWithNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("null_start", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.start(), pdFAIL);
}

// =============================================================================
// Stop: L358 (m_timer null), L361 (success/fail tracking m_started)
// =============================================================================

TEST_F(TimerCovTest, StopSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("stop_ok", 100, pdTRUE, std::move(cb));
  t.start();
  EXPECT_EQ(t.stop(), pdPASS);
}

TEST_F(TimerCovTest, StopFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("stop_fail", 100, pdTRUE, std::move(cb));
  t.start();
  EXPECT_EQ(t.stop(), pdFAIL);
}

TEST_F(TimerCovTest, StopWithNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("null_stop", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.stop(), pdFAIL);
}

// =============================================================================
// Reset: L408 (m_timer null), L411 (success/fail)
// =============================================================================

TEST_F(TimerCovTest, ResetSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerReset(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("reset_ok", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.reset(), pdPASS);
}

TEST_F(TimerCovTest, ResetFail) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerReset(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("reset_fail", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.reset(), pdFAIL);
}

TEST_F(TimerCovTest, ResetWithNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("null_reset", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.reset(), pdFAIL);
}

// =============================================================================
// Period change: L456 (m_timer null), L459 (success/fail)
// =============================================================================

TEST_F(TimerCovTest, PeriodChange) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer_handle, 2000, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("period", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.period(2000), pdPASS);
}

TEST_F(TimerCovTest, PeriodChangeWithNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("period_null", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.period(2000), pdFAIL);
}

TEST_F(TimerCovTest, PeriodIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer_handle, 500, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("period_isr", 100, pdTRUE, std::move(cb));
  auto result = t.period_isr(500);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerCovTest, PeriodIsrChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer_handle, 1000, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("period_isr_chrono", 100, pdTRUE, std::move(cb));
  auto result = t.period_isr(1s);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerCovTest, IsrWithNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("null_isr", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.start_isr().result, pdFAIL);
  EXPECT_EQ(t.stop_isr().result, pdFAIL);
  EXPECT_EQ(t.reset_isr().result, pdFAIL);
  EXPECT_EQ(t.period_isr(500).result, pdFAIL);
}

// =============================================================================
// is_active / running: L597 (m_timer null -> pdFALSE), L600 (delegate)
// =============================================================================

TEST_F(TimerCovTest, RunningActive) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerIsTimerActive(mock_timer_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("running", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.running(), pdTRUE);
}

TEST_F(TimerCovTest, RunningInactive) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerIsTimerActive(mock_timer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("not_running", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.running(), pdFALSE);
}

TEST_F(TimerCovTest, RunningNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("running_null", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.running(), pdFALSE);
}

// =============================================================================
// name, period_ticks, remaining_ticks, reload_mode
// =============================================================================

TEST_F(TimerCovTest, NameNotNull) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, pcTimerGetName(mock_timer_handle))
      .WillOnce(Return("TestName"));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("name_test", 100, pdTRUE, std::move(cb));
  EXPECT_STREQ(t.name(), "TestName");
}

TEST_F(TimerCovTest, NameNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("name_null", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.name(), nullptr);
}

TEST_F(TimerCovTest, PeriodTicks) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerGetPeriod(mock_timer_handle)).WillOnce(Return(100));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("period_ticks", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.period_ticks(), 100u);
}

TEST_F(TimerCovTest, PeriodTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("period_null", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.period_ticks(), 0u);
}

TEST_F(TimerCovTest, RemainingTicks) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerGetExpiryTime(mock_timer_handle))
      .WillOnce(Return(5000));
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(3000));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("remaining", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.remaining_ticks(), 2000u);
}

TEST_F(TimerCovTest, RemainingTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("remaining_null", 100, pdTRUE, std::move(cb));
  EXPECT_EQ(t.remaining_ticks(), 0u);
  EXPECT_EQ(t.remaining_time().count(), 0);
}

TEST_F(TimerCovTest, ReloadMode) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, vTimerSetReloadMode(mock_timer_handle, pdFALSE));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(mock_timer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("reload", 100, pdTRUE, std::move(cb));
  t.reload_mode(pdFALSE);
  EXPECT_EQ(t.reload_mode(), pdFALSE);
}

TEST_F(TimerCovTest, ReloadModeNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("reload_null", 100, pdTRUE, std::move(cb));
  t.reload_mode(pdTRUE);
}

TEST_F(TimerCovTest, ReloadModeNullHandleQuery) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  auto cb = []() {};
  da::timer t("reload_null_q", 100, pdTRUE, std::move(cb));
}

// =============================================================================
// Move constructor: L206-215 (with non-null timer -> vTimerSetTimerID)
// =============================================================================

TEST_F(TimerCovTest, MoveConstructorNonNullHandleStatic) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, vTimerSetTimerID(mock_timer_handle, _));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer src("move_test", 100, pdTRUE, std::move(cb));
  sa::timer dst = std::move(src);

  EXPECT_EQ(src.name(), nullptr);
  EXPECT_EQ(src.running(), pdFALSE);
  EXPECT_EQ(src.period_ticks(), 0u);
  EXPECT_EQ(src.start(), pdFAIL);
}

TEST_F(TimerCovTest, MoveConstructorNullHandle) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(nullptr));

  auto cb = []() {};
  sa::timer src("move_null", 100, pdTRUE, std::move(cb));
  sa::timer dst = std::move(src);
}

// =============================================================================
// SFINAE constructor with allocator args (L152-162)
// =============================================================================

struct TestMemoryRegion {
  void *allocate(size_t size) { return std::malloc(size); }
  void deallocate(void *ptr) { std::free(ptr); }
};

TEST_F(TimerCovTest, SfinaeConstructorWithExternalAllocator) {
  TestMemoryRegion region;
  auto handle = reinterpret_cast<TimerHandle_t>(0xEEEE1111);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, xTimerDelete(handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  using ext_alloc = freertos::external_sw_timer_allocator<TestMemoryRegion>;
  auto cb = []() {};
  freertos::timer<ext_alloc> t("sfinae_test", 100, pdTRUE, std::move(cb), region);
}

TEST_F(TimerCovTest, SfinaeConstructorChronoWithExternalAllocator) {
  TestMemoryRegion region;
  auto handle = reinterpret_cast<TimerHandle_t>(0xEEEE2222);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, xTimerDelete(handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  using ext_alloc = freertos::external_sw_timer_allocator<TestMemoryRegion>;
  auto cb = []() {};
  freertos::timer<ext_alloc> t("sfinae_chrono", 500ms, pdFALSE, std::move(cb), region);
}

// =============================================================================
// _ex variants (L614-728) - success and failure paths
// =============================================================================

TEST_F(TimerCovTest, StartExSuccess) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer t("start_ex_ok", 100, pdTRUE, std::move(cb));
  auto result = t.start_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerCovTest, StartExFail) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStart(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer t("start_ex_fail", 100, pdTRUE, std::move(cb));
  auto result = t.start_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerCovTest, StopExChronoFail) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerStop(mock_timer_handle, 50)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer t("stop_ex_chrono", 100, pdTRUE, std::move(cb));
  auto result = t.stop_ex(50ms);
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerCovTest, ResetExChronoFail) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerReset(mock_timer_handle, 100)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer t("reset_ex_chrono", 100, pdTRUE, std::move(cb));
  auto result = t.reset_ex(100ms);
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerCovTest, PeriodExIsrChronoFail) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer_handle, 1000, _))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  sa::timer t("period_ex_isr_chrono", 100, pdTRUE, std::move(cb));
  auto result = t.period_ex_isr(1s);
  EXPECT_FALSE(result.result.has_value());
}

// =============================================================================
// Chrono-based constructors (L173-181, L182-193)
// =============================================================================

TEST_F(TimerCovTest, ChronoConstructorDynamic) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("chrono_dyn", 500ms, pdFALSE, std::move(cb));
}

TEST_F(TimerCovTest, ChronoSfinaeConstructorDynamic) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(mock_timer_handle));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto cb = []() {};
  da::timer t("chrono_sfinae_dyn", 500ms, pdFALSE, std::move(cb));
}
/**
 * @file test_freertos_smp_affinity.cpp
 * @brief Unit tests for SMP core affinity methods on task<> and periodic_task<>
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"

#include "../include/freertos_config.hpp"
#include "../include/freertos_strong_types.hpp"
#include "../include/freertos_task.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

namespace {
void empty_task_routine() {}
void empty_on_start() {}
void empty_on_stop() {}
void empty_periodic_routine() {}
} // namespace

class SmpAffinityTest : public Test {
protected:
  void SetUp() override {
    mock = std::make_unique<StrictMock<FreeRTOSMock>>();
    g_freertos_mock = mock.get();
    mock_task_handle = reinterpret_cast<TaskHandle_t>(0xDEADBEEF);
  }

  void TearDown() override {
    g_freertos_mock = nullptr;
    mock.reset();
  }

  std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
  TaskHandle_t mock_task_handle;
};

TEST_F(SmpAffinityTest, TaskSetAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("AffinityTask", 2, empty_task_routine);

  auto mask = freertos::core_affinity_mask(0x03);
  EXPECT_CALL(*mock, vTaskCoreAffinitySet(mock_task_handle, 0x03));
  test_task.set_affinity(mask);
}

TEST_F(SmpAffinityTest, TaskClearAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("ClearAffinityTask", 2, empty_task_routine);

  auto mask = freertos::core_affinity_mask(0x01);
  EXPECT_CALL(*mock, vTaskCoreAffinityClear(mock_task_handle, 0x01));
  test_task.clear_affinity(mask);
}

TEST_F(SmpAffinityTest, TaskGetAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("GetAffinityTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGet(mock_task_handle))
      .WillOnce(Return(0x03));

  auto result = test_task.affinity();
  EXPECT_EQ(result.value(), 0x03u);
}

TEST_F(SmpAffinityTest, TaskGetAffinityISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("GetAffinityISRTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGetFromISR(mock_task_handle))
      .WillOnce(Return(0x05));

  auto result = test_task.affinity_isr();
  EXPECT_EQ(result.value(), 0x05u);
}

TEST_F(SmpAffinityTest, TaskSetAffinityWithCoreHelper) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("CoreHelperTask", 2, empty_task_routine);

  auto mask = freertos::core_affinity_mask::core(0) |
              freertos::core_affinity_mask::core(1);
  EXPECT_CALL(*mock, vTaskCoreAffinitySet(mock_task_handle, 0x03));
  test_task.set_affinity(mask);
}

TEST_F(SmpAffinityTest, TaskAffinityRoundTrip) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("RoundTripTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, vTaskCoreAffinitySet(mock_task_handle, 0x03));
  test_task.set_affinity(freertos::core_affinity_mask(0x03));

  EXPECT_CALL(*mock, ulTaskCoreAffinityGet(mock_task_handle))
      .WillOnce(Return(0x03));
  auto result = test_task.affinity();
  EXPECT_EQ(result.value(), 0x03u);

  EXPECT_CALL(*mock, vTaskCoreAffinityClear(mock_task_handle, 0x02));
  test_task.clear_affinity(freertos::core_affinity_mask::core(1));

  EXPECT_CALL(*mock, ulTaskCoreAffinityGet(mock_task_handle))
      .WillOnce(Return(0x01));
  result = test_task.affinity();
  EXPECT_EQ(result.value(), 0x01u);
}

TEST_F(SmpAffinityTest, TaskAffinityStrongTypeComparison) {
  auto mask_a = freertos::core_affinity_mask(0x03);
  auto mask_b = freertos::core_affinity_mask(0x03);
  auto mask_c = freertos::core_affinity_mask(0x01);

  EXPECT_EQ(mask_a, mask_b);
  EXPECT_NE(mask_a, mask_c);
  EXPECT_TRUE(!mask_c == false);
}

TEST_F(SmpAffinityTest, TaskAffinityBitwiseOps) {
  auto mask_a = freertos::core_affinity_mask(0x01);
  auto mask_b = freertos::core_affinity_mask(0x02);
  auto combined = mask_a | mask_b;
  EXPECT_EQ(combined.value(), 0x03u);

  auto cleared = combined & ~mask_a;
  EXPECT_EQ(cleared.value(), 0x02u);

  auto xored = mask_a ^ freertos::core_affinity_mask(0x03);
  EXPECT_EQ(xored.value(), 0x02u);
}

TEST_F(SmpAffinityTest, CoreAffinityMaskZeroIsFalsy) {
  auto zero = freertos::core_affinity_mask(0);
  EXPECT_TRUE(!zero);

  auto nonzero = freertos::core_affinity_mask(0x01);
  EXPECT_FALSE(!nonzero);
}

TEST_F(SmpAffinityTest, DynamicTaskSetAffinity) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task_handle), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  da::task<2048> test_task("DynAffinityTask", 3, empty_task_routine);

  EXPECT_CALL(*mock, vTaskCoreAffinitySet(mock_task_handle, 0x01));
  test_task.set_affinity(freertos::core_affinity_mask::core(0));
}

TEST_F(SmpAffinityTest, DynamicTaskGetAffinityISR) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task_handle), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  da::task<2048> test_task("DynAffinityISRTask", 3, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGetFromISR(mock_task_handle))
      .WillOnce(Return(0x02));
  auto result = test_task.affinity_isr();
  EXPECT_EQ(result.value(), 0x02u);
}

TEST_F(SmpAffinityTest, PeriodicTaskSetAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicAffinityTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, vTaskCoreAffinitySet(mock_task_handle, 0x03));
  periodic_task.set_affinity(freertos::core_affinity_mask(0x03));
}

TEST_F(SmpAffinityTest, PeriodicTaskClearAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicClearTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, vTaskCoreAffinityClear(mock_task_handle, 0x01));
  periodic_task.clear_affinity(freertos::core_affinity_mask::core(0));
}

TEST_F(SmpAffinityTest, PeriodicTaskGetAffinity) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicGetTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGet(mock_task_handle))
      .WillOnce(Return(0x03));

  auto result = periodic_task.affinity();
  EXPECT_EQ(result.value(), 0x03u);
}

TEST_F(SmpAffinityTest, PeriodicTaskGetAffinityISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicGetISRTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGetFromISR(mock_task_handle))
      .WillOnce(Return(0x02));

  auto result = periodic_task.affinity_isr();
  EXPECT_EQ(result.value(), 0x02u);
}

TEST_F(SmpAffinityTest, AffinityNodiscardAttribute) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("NodiscardTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskCoreAffinityGet(mock_task_handle))
      .WillOnce(Return(0x01));
  auto affinity_val = test_task.affinity();

  EXPECT_CALL(*mock, ulTaskCoreAffinityGetFromISR(mock_task_handle))
      .WillOnce(Return(0x02));
  auto affinity_isr_val = test_task.affinity_isr();

  (void)affinity_val;
  (void)affinity_isr_val;
}

TEST_F(SmpAffinityTest, CoreAffinityMaskStaticCoreMethod) {
  auto core0 = freertos::core_affinity_mask::core(0);
  EXPECT_EQ(core0.value(), 1u);

  auto core1 = freertos::core_affinity_mask::core(1);
  EXPECT_EQ(core1.value(), 2u);

  auto combined = core0 | core1;
  EXPECT_EQ(combined.value(), 3u);
}

TEST_F(SmpAffinityTest, ConfigFeatureDetectionMacro) {
#if FREERTOS_CPP_WRAPPERS_HAS_SMP_AFFINITY
  SUCCEED() << "SMP affinity feature is enabled";
#else
  SUCCEED() << "SMP affinity feature is disabled";
#endif
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
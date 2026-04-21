#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "freertos_strong_types.hpp"
#include "freertos_clock.hpp"
#include "freertos_queue_set.hpp"
#include "freertos_isr_result.hpp"

#include <chrono>

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

namespace {

class MiscBranchTest : public ::testing::Test {
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

}

// ================================================================
// STRONG TYPES BRANCH TESTS
// ================================================================

TEST_F(MiscBranchTest, PriorityAboveBelowBranches) {
  freertos::priority p0{0};
  freertos::priority p5{5};

  auto above0 = p0.above();
  EXPECT_EQ(above0.value(), 1u);

  auto above5 = p5.above();
  EXPECT_EQ(above5.value(), 6u);

  auto below0 = p0.below();
  EXPECT_EQ(below0.value(), 0u);

  auto below5 = p5.below();
  EXPECT_EQ(below5.value(), 4u);
}

TEST_F(MiscBranchTest, PriorityComparisonBothBranches) {
  freertos::priority low{2};
  freertos::priority high{5};
  freertos::priority same{5};

  EXPECT_TRUE(low < high);
  EXPECT_FALSE(high < low);
  EXPECT_FALSE(high < same);

  EXPECT_TRUE(high > low);
  EXPECT_FALSE(low > high);
  EXPECT_FALSE(high > same);

  EXPECT_TRUE(low <= high);
  EXPECT_TRUE(high <= same);
  EXPECT_FALSE(high <= low);

  EXPECT_TRUE(high >= low);
  EXPECT_TRUE(high >= same);
  EXPECT_FALSE(low >= high);

  EXPECT_TRUE(high == same);
  EXPECT_FALSE(high == low);

  EXPECT_TRUE(high != low);
  EXPECT_FALSE(high != same);
}

TEST_F(MiscBranchTest, EventBitsBitwiseOpsBranches) {
  freertos::event_bits a{0x0F};
  freertos::event_bits b{0x55};

  auto or_result = a | b;
  EXPECT_EQ(or_result.value(), EventBits_t{0x0F | 0x55});

  auto and_result = a & b;
  EXPECT_EQ(and_result.value(), EventBits_t{0x0F & 0x55});

  auto xor_result = a ^ b;
  EXPECT_EQ(xor_result.value(), EventBits_t{0x0F ^ 0x55});

  auto not_result = ~a;
  EXPECT_EQ(not_result.value(), static_cast<EventBits_t>(~0x0F));
}

TEST_F(MiscBranchTest, EventBitsCompoundAssignBranches) {
  freertos::event_bits a{0x01};
  a |= freertos::event_bits{0x02};
  EXPECT_EQ(a.value(), EventBits_t{0x03});

  a &= freertos::event_bits{0x02};
  EXPECT_EQ(a.value(), EventBits_t{0x02});

  a ^= freertos::event_bits{0x02};
  EXPECT_EQ(a.value(), EventBits_t{0x00});
}

TEST_F(MiscBranchTest, EventBitsComparisonBranches) {
  freertos::event_bits e1{0x01};
  freertos::event_bits e2{0x02};

  EXPECT_TRUE(e1 == e1);
  EXPECT_FALSE(e1 == e2);

  EXPECT_TRUE(e1 != e2);
  EXPECT_FALSE(e1 != e1);

  EXPECT_TRUE(!freertos::event_bits{0});
  EXPECT_FALSE(!e1);

  EXPECT_TRUE(e1.any());
  EXPECT_FALSE(freertos::event_bits{0}.any());

  EXPECT_TRUE(freertos::event_bits{0}.none());
  EXPECT_FALSE(e1.none());
}

TEST_F(MiscBranchTest, TickCountArithmeticBranches) {
  freertos::tick_count a{10};
  freertos::tick_count b{3};

  auto sum = a + b;
  EXPECT_EQ(sum.value(), TickType_t{13});

  auto diff = a - b;
  EXPECT_EQ(diff.value(), TickType_t{7});

  freertos::tick_count c{5};
  c += freertos::tick_count{3};
  EXPECT_EQ(c.value(), TickType_t{8});

  c -= freertos::tick_count{2};
  EXPECT_EQ(c.value(), TickType_t{6});
}

TEST_F(MiscBranchTest, TickCountComparisonBothBranches) {
  freertos::tick_count low{2};
  freertos::tick_count high{5};
  freertos::tick_count same{5};

  EXPECT_TRUE(low < high);
  EXPECT_FALSE(high < low);
  EXPECT_FALSE(high < same);

  EXPECT_TRUE(high > low);
  EXPECT_FALSE(low > high);
  EXPECT_FALSE(high > same);

  EXPECT_TRUE(low <= high);
  EXPECT_TRUE(high <= same);
  EXPECT_FALSE(high <= low);

  EXPECT_TRUE(high >= low);
  EXPECT_TRUE(high >= same);
  EXPECT_FALSE(low >= high);

  EXPECT_TRUE(high == same);
  EXPECT_FALSE(high == low);

  EXPECT_TRUE(high != low);
  EXPECT_FALSE(high != same);
}

TEST_F(MiscBranchTest, CoreAffinityMaskBitwiseOpsBranches) {
  freertos::core_affinity_mask a{0x0F};
  freertos::core_affinity_mask b{0x55};

  auto or_result = a | b;
  EXPECT_EQ(or_result.value(), UBaseType_t{0x0F | 0x55});

  auto and_result = a & b;
  EXPECT_EQ(and_result.value(), UBaseType_t{0x0F & 0x55});

  auto xor_result = a ^ b;
  EXPECT_EQ(xor_result.value(), UBaseType_t{0x0F ^ 0x55});

  auto not_result = ~a;
  EXPECT_EQ(not_result.value(), static_cast<UBaseType_t>(~0x0F));
}

TEST_F(MiscBranchTest, CoreAffinityMaskCompoundAssignBranches) {
  freertos::core_affinity_mask a{0x01};
  a |= freertos::core_affinity_mask{0x02};
  EXPECT_EQ(a.value(), UBaseType_t{0x03});

  a &= freertos::core_affinity_mask{0x02};
  EXPECT_EQ(a.value(), UBaseType_t{0x02});

  a ^= freertos::core_affinity_mask{0x02};
  EXPECT_EQ(a.value(), UBaseType_t{0x00});
}

TEST_F(MiscBranchTest, CoreAffinityMaskComparisonBranches) {
  freertos::core_affinity_mask a{0x01};
  freertos::core_affinity_mask b{0x02};

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a == b);

  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a != a);

  EXPECT_TRUE(!freertos::core_affinity_mask{0});
  EXPECT_FALSE(!a);
}

TEST_F(MiscBranchTest, CoreAffinityMaskCoreFactoryBranches) {
  auto core0 = freertos::core_affinity_mask::core(0);
  EXPECT_EQ(core0.value(), UBaseType_t{1});

  auto core3 = freertos::core_affinity_mask::core(3);
  EXPECT_EQ(core3.value(), UBaseType_t{8});

  auto core7 = freertos::core_affinity_mask::core(7);
  EXPECT_EQ(core7.value(), UBaseType_t{128});
}

// ================================================================
// CLOCK BRANCH TESTS
// ================================================================

TEST_F(MiscBranchTest, TickTimerNowBranch) {
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(TickType_t{100}));
  auto tp = freertos::tick_timer::now();
  EXPECT_EQ(tp.time_since_epoch().count(), TickType_t{100});
}

TEST_F(MiscBranchTest, TickTimerNowZero) {
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(TickType_t{0}));
  auto tp = freertos::tick_timer::now();
  EXPECT_EQ(tp.time_since_epoch().count(), TickType_t{0});
}

TEST_F(MiscBranchTest, TickTimerNowLarge) {
  EXPECT_CALL(*mock, xTaskGetTickCount())
      .WillOnce(Return(TickType_t{0xFFFFFFFE}));
  auto tp = freertos::tick_timer::now();
  EXPECT_EQ(tp.time_since_epoch().count(), TickType_t{0xFFFFFFFE});
}

TEST_F(MiscBranchTest, ToTicksZeroDuration) {
  auto ticks = freertos::to_ticks(std::chrono::milliseconds{0});
  EXPECT_EQ(ticks, TickType_t{0});
}

TEST_F(MiscBranchTest, ToTicksLargeDuration) {
  auto ticks = freertos::to_ticks(std::chrono::seconds{60});
  EXPECT_EQ(ticks, TickType_t{60000});
}

TEST_F(MiscBranchTest, ToTicksSubTickTruncation) {
  auto ticks = freertos::to_ticks(std::chrono::microseconds{500});
  EXPECT_EQ(ticks, TickType_t{0});
}

TEST_F(MiscBranchTest, ToTicksExactTick) {
  auto ticks = freertos::to_ticks(std::chrono::milliseconds{1});
  EXPECT_EQ(ticks, TickType_t{1});
}

TEST_F(MiscBranchTest, ToTicksHours) {
  auto ticks = freertos::to_ticks(std::chrono::hours{1});
  EXPECT_EQ(ticks, TickType_t{3600000});
}

TEST_F(MiscBranchTest, ToDurationZeroTicks) {
  auto ms = freertos::to_duration(TickType_t{0});
  EXPECT_EQ(ms.count(), 0);
}

TEST_F(MiscBranchTest, ToDurationLargeTicks) {
  auto ms = freertos::to_duration(TickType_t{10000});
  EXPECT_EQ(ms.count(), 10000);
}

TEST_F(MiscBranchTest, ToDurationAsMicroseconds) {
  auto us = freertos::to_duration<std::chrono::microseconds>(TickType_t{1});
  EXPECT_EQ(us.count(), 1000);
}

TEST_F(MiscBranchTest, ToDurationAsSeconds) {
  auto sec = freertos::to_duration<std::chrono::seconds>(TickType_t{5000});
  EXPECT_EQ(sec.count(), 5);
}

TEST_F(MiscBranchTest, ToTicksToDurationRoundTrip) {
  TickType_t original{42};
  auto rt = freertos::to_ticks(freertos::to_duration(original));
  EXPECT_EQ(rt, original);
}

// ================================================================
// QUEUE SET BRANCH TESTS
// ================================================================

TEST_F(MiscBranchTest, QueueSetDestructorNonNullHandle) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x5000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
}

TEST_F(MiscBranchTest, QueueSetDestructorNullHandle) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(static_cast<QueueSetHandle_t>(nullptr)));

  freertos::da::queue_set qs(5);
}

TEST_F(MiscBranchTest, QueueSetMoveAssignmentSelf) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x6000);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs(3);
  qs = std::move(qs);
}

TEST_F(MiscBranchTest, QueueSetMoveAssignmentDifferent) {
  QueueSetHandle_t handle1 = reinterpret_cast<QueueSetHandle_t>(0x6001);
  QueueSetHandle_t handle2 = reinterpret_cast<QueueSetHandle_t>(0x6002);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle1));
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vQueueDelete(handle1));
  EXPECT_CALL(*mock, vQueueDelete(handle2));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(5);
  qs1 = std::move(qs2);
}

TEST_F(MiscBranchTest, QueueSetAddSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x7000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x7001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueAddToSet(member, handle)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  EXPECT_EQ(qs.add(member), pdPASS);
}

TEST_F(MiscBranchTest, QueueSetAddFailure) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x7000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x7001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueAddToSet(member, handle)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  EXPECT_EQ(qs.add(member), pdFAIL);
}

TEST_F(MiscBranchTest, QueueSetRemoveSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x7000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x7001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member, handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  EXPECT_EQ(qs.remove(member), pdPASS);
}

TEST_F(MiscBranchTest, QueueSetRemoveFailure) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x7000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x7001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member, handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  EXPECT_EQ(qs.remove(member), pdFAIL);
}

TEST_F(MiscBranchTest, QueueSetSelectWithTicksFound) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x8000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x8001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 100))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  EXPECT_EQ(qs.select(100), member);
}

TEST_F(MiscBranchTest, QueueSetSelectWithTicksTimeout) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x8000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 100))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  EXPECT_EQ(qs.select(100), nullptr);
}

TEST_F(MiscBranchTest, QueueSetSelectISRFound) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x8000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x8001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(handle, _))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_isr();
  EXPECT_EQ(result.result, member);
}

TEST_F(MiscBranchTest, QueueSetSelectISRNotFound) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x8000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(handle, _))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_isr();
  EXPECT_EQ(result.result, nullptr);
}

TEST_F(MiscBranchTest, QueueSetSelectExISRNonNullResult) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x9000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0x9001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(handle, _))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), member);
}

TEST_F(MiscBranchTest, QueueSetSelectExISRNullResult) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0x9000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(handle, _))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(MiscBranchTest, QueueSetAddExSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xA000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xA001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueAddToSet(member, handle)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  auto result = qs.add_ex(member);
  EXPECT_TRUE(result.has_value());
}

TEST_F(MiscBranchTest, QueueSetAddExFail) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xA000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xA001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueAddToSet(member, handle)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  auto result = qs.add_ex(member);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(MiscBranchTest, QueueSetRemoveExSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xB000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xB001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member, handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  auto result = qs.remove_ex(member);
  EXPECT_TRUE(result.has_value());
}

TEST_F(MiscBranchTest, QueueSetRemoveExFail) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xB000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xB001);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueRemoveFromSet(member, handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(3);
  auto result = qs.remove_ex(member);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(MiscBranchTest, QueueSetSelectExWithTicksSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xC000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xC001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 100))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), member);
}

TEST_F(MiscBranchTest, QueueSetSelectExWithTicksTimeout) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xC000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 100))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MiscBranchTest, QueueSetSelectExWithTicksWouldBlock) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xC000);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 0))
      .WillOnce(Return(static_cast<QueueSetMemberHandle_t>(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MiscBranchTest, QueueSetSelectChronoDuration) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xD000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xD001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, 500))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select(std::chrono::milliseconds(500));
  EXPECT_EQ(result, member);
}

TEST_F(MiscBranchTest, QueueSetSelectExChronoSuccess) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xE000);
  QueueSetMemberHandle_t member =
      reinterpret_cast<QueueSetMemberHandle_t>(0xE001);
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(handle, _))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(std::chrono::milliseconds(200));
  EXPECT_TRUE(result.has_value());
}

TEST_F(MiscBranchTest, QueueSetMoveConstruction) {
  QueueSetHandle_t handle = reinterpret_cast<QueueSetHandle_t>(0xF000);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(std::move(qs1));
}

TEST_F(MiscBranchTest, QueueSetSwap) {
  QueueSetHandle_t handle1 = reinterpret_cast<QueueSetHandle_t>(0xF001);
  QueueSetHandle_t handle2 = reinterpret_cast<QueueSetHandle_t>(0xF002);
  EXPECT_CALL(*mock, xQueueCreateSet(3)).WillOnce(Return(handle1));
  EXPECT_CALL(*mock, xQueueCreateSet(5)).WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vQueueDelete(handle1));
  EXPECT_CALL(*mock, vQueueDelete(handle2));

  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs1(3);
  freertos::queue_set<freertos::dynamic_queue_set_allocator> qs2(5);
  qs1.swap(qs2);
}
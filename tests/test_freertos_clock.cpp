#include "FreeRTOS.h"
#include "freertos_clock.hpp"
#include "freertos_mocks.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <type_traits>

using namespace freertos;

static_assert(std::is_trivially_copyable<tick_timer>::value,
              "tick_timer must be trivially copyable");
static_assert(tick_timer::is_steady, "tick_timer must be steady");
static_assert(std::is_same<tick_timer::rep, TickType_t>::value,
              "tick_timer::rep must be TickType_t");
static_assert(std::is_same<tick_timer::period, std::ratio<1, configTICK_RATE_HZ>>::value,
              "tick_timer::period must be std::ratio<1, configTICK_RATE_HZ>");
static_assert(std::is_same<tick_timer::duration,
                           std::chrono::duration<TickType_t, std::ratio<1, configTICK_RATE_HZ>>>::value,
              "tick_timer::duration must use correct period");
static_assert(std::is_same<tick_timer::time_point,
                           std::chrono::time_point<tick_timer>>::value,
              "tick_timer::time_point must be std::chrono::time_point<tick_timer>");

TEST(TickTimerTest, MeetsTrivialClockRequirements) {
  EXPECT_TRUE(std::is_trivially_copyable<tick_timer>::value);
  EXPECT_TRUE(tick_timer::is_steady);
}

TEST(TickTimerTest, NowCallsXTaskGetTickCount) {
  auto mock = FreeRTOSMockInstance::getInstance();
  EXPECT_CALL(*mock, xTaskGetTickCount())
      .WillOnce(::testing::Return(TickType_t{42}));
  auto tp = tick_timer::now();
  EXPECT_EQ(tp.time_since_epoch().count(), TickType_t{42});
  FreeRTOSMockInstance::resetInstance();
}

TEST(TickTimerTest, DurationHasCorrectPeriod) {
  using p = tick_timer::duration::period;
  EXPECT_EQ(p::num, 1);
  EXPECT_EQ(p::den, configTICK_RATE_HZ);
}

TEST(TickTimerTest, TimePointIsChronoTimePoint) {
  EXPECT_TRUE((std::is_same<tick_timer::time_point,
                             std::chrono::time_point<tick_timer>>::value));
}

TEST(ToTicksTest, ConvertsMillisecondsToTicks) {
  auto ticks = to_ticks(std::chrono::milliseconds{5});
  EXPECT_EQ(ticks, TickType_t{5});
}

TEST(ToTicksTest, ConvertsSubMillisecondDuration) {
  auto ticks = to_ticks(std::chrono::microseconds{500});
  EXPECT_EQ(ticks, TickType_t{0});
}

TEST(ToTicksTest, HandlesZeroDuration) {
  auto ticks = to_ticks(std::chrono::milliseconds{0});
  EXPECT_EQ(ticks, TickType_t{0});
}

TEST(ToDurationTest, ConvertsTicksToMilliseconds) {
  auto ms = to_duration(TickType_t{5});
  EXPECT_EQ(ms.count(), std::chrono::milliseconds::rep{5});
}

TEST(ToDurationTest, ConvertsTicksToDifferentDurationTypes) {
  auto us = to_duration<std::chrono::microseconds>(TickType_t{1});
  EXPECT_EQ(us.count(), std::chrono::microseconds::rep{1000});
}

TEST(ToTicksToDurationRoundTrip, RoundTripPreservesTicks) {
  TickType_t original{7};
  auto ticks = to_ticks(to_duration(original));
  EXPECT_EQ(ticks, original);
}
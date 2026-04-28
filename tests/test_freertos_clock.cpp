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

// =============================================================================
// to_ticks_saturating: regression tests for #303 (silent-corruption window)
// -----------------------------------------------------------------------------
// At 1 kHz tick rate with a 32-bit TickType_t, the macro pdMS_TO_TICKS used to
// wrap silently for ms > ~4.3M while saturation only kicked in at ms > ~4.29B.
// The rewritten helper performs the multiplication in 64-bit and saturates at
// portMAX_DELAY directly, closing that window. These tests pin down the
// boundary cases and the behavior at and above the saturation point.
// =============================================================================

TEST(ToTicksSaturatingTest, ZeroDurationReturnsZero) {
  EXPECT_EQ(to_ticks_saturating(std::chrono::milliseconds{0}), TickType_t{0});
}

TEST(ToTicksSaturatingTest, OneMillisecondReturnsOneTickAt1kHz) {
  // configTICK_RATE_HZ is 1000 in the mock layer, so 1 ms == 1 tick.
  static_assert(configTICK_RATE_HZ == 1000,
                "test assumes 1 kHz tick rate from the mock FreeRTOSConfig");
  EXPECT_EQ(to_ticks_saturating(std::chrono::milliseconds{1}), TickType_t{1});
}

TEST(ToTicksSaturatingTest, NegativeDurationReturnsZero) {
  EXPECT_EQ(to_ticks_saturating(std::chrono::milliseconds{-1}), TickType_t{0});
  EXPECT_EQ(to_ticks_saturating(std::chrono::milliseconds{-1000000}),
            TickType_t{0});
}

TEST(ToTicksSaturatingTest, AtPdMSToTicksOverflowBoundaryDoesNotWrap) {
  // The pdMS_TO_TICKS macro used to wrap when (ms * configTICK_RATE_HZ) overflowed
  // its native arithmetic type. At configTICK_RATE_HZ == 1000, that boundary is
  // around (UINT32_MAX / 1000) ms ≈ 4'294'967 ms. Just above this point the
  // 32-bit product wraps to a small value; the 64-bit helper must not.
  constexpr int64_t boundary_ms = 4'294'968; // one ms past the wrap point
  const auto result =
      to_ticks_saturating(std::chrono::milliseconds{boundary_ms});
  // Expected raw: 4'294'968'000, which exceeds portMAX_DELAY (0xFFFFFFFF =
  // 4'294'967'295), so we must saturate.
  EXPECT_EQ(result, static_cast<TickType_t>(portMAX_DELAY));
}

TEST(ToTicksSaturatingTest, JustBelowBoundaryComputesNormally) {
  // 4'294'967 ms * 1000 = 4'294'967'000 ticks, which fits in TickType_t.
  constexpr int64_t below_ms = 4'294'967;
  const auto result =
      to_ticks_saturating(std::chrono::milliseconds{below_ms});
  EXPECT_EQ(result, static_cast<TickType_t>(4'294'967'000ULL));
}

TEST(ToTicksSaturatingTest, MuchLargerThanOverflowSaturates) {
  // 1 hour expressed as ms is ~3.6M, well within range — pick something
  // dramatic instead: a year of milliseconds vastly overflows TickType_t.
  const auto one_year_ms = std::chrono::hours{24 * 365};
  EXPECT_EQ(to_ticks_saturating(one_year_ms),
            static_cast<TickType_t>(portMAX_DELAY));
}

TEST(ToTicksSaturatingTest, MaxDurationSaturates) {
  // std::chrono::seconds::max() converted to ms is enormous and must saturate
  // without invoking UB.
  const auto huge = std::chrono::seconds::max();
  EXPECT_EQ(to_ticks_saturating(huge),
            static_cast<TickType_t>(portMAX_DELAY));
}

TEST(ToTicksSaturatingTest, MinDurationReturnsZero) {
  // Negative-extreme inputs must clamp at zero.
  const auto huge_negative = std::chrono::seconds::min();
  EXPECT_EQ(to_ticks_saturating(huge_negative), TickType_t{0});
}
#include "FreeRTOS.h"
#include "freertos_strong_types.hpp"
#include <gtest/gtest.h>

using namespace freertos;

static_assert(priority{5} == priority{5}, "");
static_assert(priority{5} != priority{3}, "");
static_assert(priority{3} < priority{5}, "");
static_assert(priority{3} <= priority{5}, "");
static_assert(priority{5} > priority{3}, "");
static_assert(priority{5} >= priority{3}, "");
static_assert(priority{3} <= priority{3}, "");
static_assert(priority{3} >= priority{3}, "");
static_assert(priority{4}.above() == priority{5}, "");
static_assert(priority{4}.below() == priority{3}, "");
static_assert(priority{0}.below() == priority{0}, "");
static_assert(priority{0}.value() == 0, "");
static_assert(priority{7}.value() == 7, "");

static_assert((event_bits{0x01} | event_bits{0x02}) == event_bits{0x03}, "");
static_assert((event_bits{0x03} & event_bits{0x02}) == event_bits{0x02}, "");
static_assert((event_bits{0x03} ^ event_bits{0x01}) == event_bits{0x02}, "");
static_assert(~event_bits{0xFF} == event_bits{static_cast<EventBits_t>(~0xFF)},
              "");
static_assert(event_bits{0}.none(), "");
static_assert(!event_bits{0}, "");
static_assert(event_bits{1}.any(), "");
static_assert(event_bits{1}.value() == 1, "");

static_assert(tick_count{5} == tick_count{5}, "");
static_assert(tick_count{5} != tick_count{3}, "");
static_assert(tick_count{3} < tick_count{5}, "");
static_assert(tick_count{3} <= tick_count{5}, "");
static_assert(tick_count{5} > tick_count{3}, "");
static_assert(tick_count{5} >= tick_count{3}, "");
static_assert(tick_count{2} + tick_count{3} == tick_count{5}, "");
static_assert(tick_count{5} - tick_count{3} == tick_count{2}, "");
static_assert(tick_count{0}.value() == 0, "");

static_assert((core_affinity_mask{0x01} | core_affinity_mask{0x02}) ==
                  core_affinity_mask{0x03},
              "");
static_assert((core_affinity_mask{0x03} & core_affinity_mask{0x02}) ==
                  core_affinity_mask{0x02},
              "");
static_assert((core_affinity_mask{0x03} ^ core_affinity_mask{0x01}) ==
                  core_affinity_mask{0x02},
              "");
static_assert(!core_affinity_mask{0}, "");
static_assert(core_affinity_mask::core(0) == core_affinity_mask{1}, "");
static_assert(core_affinity_mask::core(1) == core_affinity_mask{2}, "");
static_assert(core_affinity_mask::core(2) == core_affinity_mask{4}, "");

TEST(PriorityTest, DefaultConstruction) {
  priority p;
  EXPECT_EQ(p.value(), 0u);
}

TEST(PriorityTest, ExplicitValueConstruction) {
  priority p{5};
  EXPECT_EQ(p.value(), 5u);
}

TEST(PriorityTest, ValueAccessor) {
  priority p{42};
  EXPECT_EQ(p.value(), 42u);
}

TEST(PriorityTest, Equality) {
  EXPECT_TRUE(priority{5} == priority{5});
  EXPECT_FALSE(priority{5} == priority{3});
}

TEST(PriorityTest, Inequality) {
  EXPECT_TRUE(priority{5} != priority{3});
  EXPECT_FALSE(priority{5} != priority{5});
}

TEST(PriorityTest, LessThan) {
  EXPECT_TRUE(priority{3} < priority{5});
  EXPECT_FALSE(priority{5} < priority{3});
  EXPECT_FALSE(priority{5} < priority{5});
}

TEST(PriorityTest, LessEqual) {
  EXPECT_TRUE(priority{3} <= priority{5});
  EXPECT_TRUE(priority{5} <= priority{5});
  EXPECT_FALSE(priority{5} <= priority{3});
}

TEST(PriorityTest, GreaterThan) {
  EXPECT_TRUE(priority{5} > priority{3});
  EXPECT_FALSE(priority{3} > priority{5});
  EXPECT_FALSE(priority{5} > priority{5});
}

TEST(PriorityTest, GreaterEqual) {
  EXPECT_TRUE(priority{5} >= priority{3});
  EXPECT_TRUE(priority{5} >= priority{5});
  EXPECT_FALSE(priority{3} >= priority{5});
}

TEST(PriorityTest, Above) {
  priority p{4};
  EXPECT_EQ(p.above(), priority{5});
  EXPECT_EQ(priority{0}.above(), priority{1});
}

TEST(PriorityTest, Below) {
  priority p{4};
  EXPECT_EQ(p.below(), priority{3});
  EXPECT_EQ(priority{0}.below(), priority{0});
  EXPECT_EQ(priority{1}.below(), priority{0});
}

TEST(EventBitsTest, DefaultConstruction) {
  event_bits e;
  EXPECT_EQ(e.value(), EventBits_t{0});
}

TEST(EventBitsTest, ExplicitValueConstruction) {
  event_bits e{0x0F};
  EXPECT_EQ(e.value(), EventBits_t{0x0F});
}

TEST(EventBitsTest, ValueAccessor) {
  event_bits e{0xAB};
  EXPECT_EQ(e.value(), EventBits_t{0xAB});
}

TEST(EventBitsTest, BitwiseOr) {
  event_bits a{0x01};
  event_bits b{0x02};
  EXPECT_EQ(a | b, event_bits{0x03});
}

TEST(EventBitsTest, BitwiseAnd) {
  event_bits a{0x03};
  event_bits b{0x02};
  EXPECT_EQ(a & b, event_bits{0x02});
}

TEST(EventBitsTest, BitwiseXor) {
  event_bits a{0x03};
  event_bits b{0x01};
  EXPECT_EQ(a ^ b, event_bits{0x02});
}

TEST(EventBitsTest, BitwiseNot) {
  event_bits e{0xFF};
  EXPECT_EQ(~e, event_bits{static_cast<EventBits_t>(~0xFF)});
}

TEST(EventBitsTest, BitwiseOrAssign) {
  event_bits a{0x01};
  a |= event_bits{0x02};
  EXPECT_EQ(a, event_bits{0x03});
}

TEST(EventBitsTest, BitwiseAndAssign) {
  event_bits a{0x03};
  a &= event_bits{0x02};
  EXPECT_EQ(a, event_bits{0x02});
}

TEST(EventBitsTest, BitwiseXorAssign) {
  event_bits a{0x03};
  a ^= event_bits{0x01};
  EXPECT_EQ(a, event_bits{0x02});
}

TEST(EventBitsTest, Equality) {
  EXPECT_TRUE(event_bits{5} == event_bits{5});
  EXPECT_FALSE(event_bits{5} == event_bits{3});
}

TEST(EventBitsTest, Inequality) {
  EXPECT_TRUE(event_bits{5} != event_bits{3});
  EXPECT_FALSE(event_bits{5} != event_bits{5});
}

TEST(EventBitsTest, OperatorNot) {
  EXPECT_TRUE(!event_bits{0});
  EXPECT_FALSE(!event_bits{1});
}

TEST(EventBitsTest, Any) {
  EXPECT_TRUE(event_bits{1}.any());
  EXPECT_FALSE(event_bits{0}.any());
}

TEST(EventBitsTest, None) {
  EXPECT_TRUE(event_bits{0}.none());
  EXPECT_FALSE(event_bits{1}.none());
}

TEST(TickCountTest, DefaultConstruction) {
  tick_count t;
  EXPECT_EQ(t.value(), TickType_t{0});
}

TEST(TickCountTest, ExplicitValueConstruction) {
  tick_count t{100};
  EXPECT_EQ(t.value(), TickType_t{100});
}

TEST(TickCountTest, ValueAccessor) {
  tick_count t{42};
  EXPECT_EQ(t.value(), TickType_t{42});
}

TEST(TickCountTest, Addition) {
  tick_count a{2};
  tick_count b{3};
  EXPECT_EQ(a + b, tick_count{5});
}

TEST(TickCountTest, Subtraction) {
  tick_count a{5};
  tick_count b{3};
  EXPECT_EQ(a - b, tick_count{2});
}

TEST(TickCountTest, PlusAssign) {
  tick_count a{2};
  a += tick_count{3};
  EXPECT_EQ(a, tick_count{5});
}

TEST(TickCountTest, MinusAssign) {
  tick_count a{5};
  a -= tick_count{3};
  EXPECT_EQ(a, tick_count{2});
}

TEST(TickCountTest, Equality) {
  EXPECT_TRUE(tick_count{5} == tick_count{5});
  EXPECT_FALSE(tick_count{5} == tick_count{3});
}

TEST(TickCountTest, Inequality) {
  EXPECT_TRUE(tick_count{5} != tick_count{3});
  EXPECT_FALSE(tick_count{5} != tick_count{5});
}

TEST(TickCountTest, LessThan) {
  EXPECT_TRUE(tick_count{3} < tick_count{5});
  EXPECT_FALSE(tick_count{5} < tick_count{3});
  EXPECT_FALSE(tick_count{5} < tick_count{5});
}

TEST(TickCountTest, LessEqual) {
  EXPECT_TRUE(tick_count{3} <= tick_count{5});
  EXPECT_TRUE(tick_count{5} <= tick_count{5});
  EXPECT_FALSE(tick_count{5} <= tick_count{3});
}

TEST(TickCountTest, GreaterThan) {
  EXPECT_TRUE(tick_count{5} > tick_count{3});
  EXPECT_FALSE(tick_count{3} > tick_count{5});
  EXPECT_FALSE(tick_count{5} > tick_count{5});
}

TEST(TickCountTest, GreaterEqual) {
  EXPECT_TRUE(tick_count{5} >= tick_count{3});
  EXPECT_TRUE(tick_count{5} >= tick_count{5});
  EXPECT_FALSE(tick_count{3} >= tick_count{5});
}

TEST(CoreAffinityMaskTest, DefaultConstruction) {
  core_affinity_mask m;
  EXPECT_EQ(m.value(), UBaseType_t{0});
}

TEST(CoreAffinityMaskTest, ExplicitValueConstruction) {
  core_affinity_mask m{0x03};
  EXPECT_EQ(m.value(), UBaseType_t{0x03});
}

TEST(CoreAffinityMaskTest, ValueAccessor) {
  core_affinity_mask m{0xAB};
  EXPECT_EQ(m.value(), UBaseType_t{0xAB});
}

TEST(CoreAffinityMaskTest, BitwiseOr) {
  core_affinity_mask a{0x01};
  core_affinity_mask b{0x02};
  EXPECT_EQ(a | b, core_affinity_mask{0x03});
}

TEST(CoreAffinityMaskTest, BitwiseAnd) {
  core_affinity_mask a{0x03};
  core_affinity_mask b{0x02};
  EXPECT_EQ(a & b, core_affinity_mask{0x02});
}

TEST(CoreAffinityMaskTest, BitwiseXor) {
  core_affinity_mask a{0x03};
  core_affinity_mask b{0x01};
  EXPECT_EQ(a ^ b, core_affinity_mask{0x02});
}

TEST(CoreAffinityMaskTest, BitwiseNot) {
  core_affinity_mask m{0xFF};
  EXPECT_EQ(~m, core_affinity_mask{static_cast<UBaseType_t>(~0xFF)});
}

TEST(CoreAffinityMaskTest, BitwiseOrAssign) {
  core_affinity_mask a{0x01};
  a |= core_affinity_mask{0x02};
  EXPECT_EQ(a, core_affinity_mask{0x03});
}

TEST(CoreAffinityMaskTest, BitwiseAndAssign) {
  core_affinity_mask a{0x03};
  a &= core_affinity_mask{0x02};
  EXPECT_EQ(a, core_affinity_mask{0x02});
}

TEST(CoreAffinityMaskTest, BitwiseXorAssign) {
  core_affinity_mask a{0x03};
  a ^= core_affinity_mask{0x01};
  EXPECT_EQ(a, core_affinity_mask{0x02});
}

TEST(CoreAffinityMaskTest, Equality) {
  EXPECT_TRUE(core_affinity_mask{5} == core_affinity_mask{5});
  EXPECT_FALSE(core_affinity_mask{5} == core_affinity_mask{3});
}

TEST(CoreAffinityMaskTest, Inequality) {
  EXPECT_TRUE(core_affinity_mask{5} != core_affinity_mask{3});
  EXPECT_FALSE(core_affinity_mask{5} != core_affinity_mask{5});
}

TEST(CoreAffinityMaskTest, OperatorNot) {
  EXPECT_TRUE(!core_affinity_mask{0});
  EXPECT_FALSE(!core_affinity_mask{1});
}

TEST(CoreAffinityMaskTest, CoreFactoryMethod) {
  EXPECT_EQ(core_affinity_mask::core(0), core_affinity_mask{1});
  EXPECT_EQ(core_affinity_mask::core(1), core_affinity_mask{2});
  EXPECT_EQ(core_affinity_mask::core(2), core_affinity_mask{4});
  EXPECT_EQ(core_affinity_mask::core(3), core_affinity_mask{8});
}

TEST(CoreAffinityMaskTest, CoreCombination) {
  auto mask = core_affinity_mask::core(0) | core_affinity_mask::core(1);
  EXPECT_EQ(mask, core_affinity_mask{3});
}
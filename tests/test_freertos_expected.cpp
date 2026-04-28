#include <freertos_expected.hpp>

#include <gtest/gtest.h>
#include <string>

namespace {

using freertos::error;
using freertos::expected;
using freertos::unexpect;
using freertos::unexpected;

TEST(ExpectedTest, ErrorEnumValues) {
  EXPECT_EQ(static_cast<uint8_t>(error::ok), 0);
  EXPECT_EQ(static_cast<uint8_t>(error::timeout), 1);
  EXPECT_EQ(static_cast<uint8_t>(error::would_block), 2);
  EXPECT_EQ(static_cast<uint8_t>(error::queue_full), 3);
  EXPECT_EQ(static_cast<uint8_t>(error::queue_empty), 4);
  EXPECT_EQ(static_cast<uint8_t>(error::semaphore_not_owned), 5);
  EXPECT_EQ(static_cast<uint8_t>(error::invalid_handle), 6);
  EXPECT_EQ(static_cast<uint8_t>(error::out_of_memory), 7);
  EXPECT_EQ(static_cast<uint8_t>(error::invalid_parameter), 8);
}

TEST(ExpectedTest, UnexpectedConstruction) {
  unexpected<error> u(error::timeout);
  EXPECT_EQ(u.value(), error::timeout);
}

TEST(ExpectedTest, UnexpectedEquality) {
  unexpected<error> u1(error::timeout);
  unexpected<error> u2(error::timeout);
  unexpected<error> u3(error::queue_full);
  EXPECT_EQ(u1, u2);
  EXPECT_NE(u1, u3);
}

TEST(ExpectedTest, ExpectedWithValue) {
  expected<int, error> e(42);
  EXPECT_TRUE(e.has_value());
  EXPECT_TRUE(static_cast<bool>(e));
  EXPECT_EQ(e.value(), 42);
  EXPECT_EQ(*e, 42);
}

TEST(ExpectedTest, ExpectedWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e.has_value());
  EXPECT_FALSE(static_cast<bool>(e));
  EXPECT_EQ(e.error(), error::timeout);
}

TEST(ExpectedTest, ExpectedArrow) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_EQ(e->size(), 5u);
  EXPECT_EQ(e->c_str(), std::string("hello"));
}

TEST(ExpectedTest, ExpectedCopyConstruction) {
  expected<int, error> e1(10);
  expected<int, error> e2(e1);
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 10);

  expected<int, error> e3{unexpected<error>(error::invalid_handle)};
  expected<int, error> e4(e3);
  EXPECT_FALSE(e4.has_value());
  EXPECT_EQ(e4.error(), error::invalid_handle);
}

TEST(ExpectedTest, ExpectedMoveConstruction) {
  expected<int, error> e1(10);
  expected<int, error> e2(std::move(e1));
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 10);
}

TEST(ExpectedTest, ExpectedCopyAssignment) {
  expected<int, error> e1(10);
  expected<int, error> e2{unexpected<error>(error::timeout)};
  e2 = e1;
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 10);
}

TEST(ExpectedTest, ExpectedMoveAssignment) {
  expected<int, error> e1(10);
  expected<int, error> e2{unexpected<error>(error::timeout)};
  e2 = std::move(e1);
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 10);
}

TEST(ExpectedTest, ExpectedUnexpectConstruction) {
  expected<int, error> e(unexpect, error::out_of_memory);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST(ExpectedTest, ExpectedAndThenWithValue) {
  expected<int, error> e(5);
  auto result = e.and_then([](int v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

TEST(ExpectedTest, ExpectedAndThenWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](int v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST(ExpectedTest, ExpectedOrElseWithValue) {
  expected<int, error> e(5);
  auto result = e.or_else(
      [](error) -> expected<int, error> { return expected<int, error>(99); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST(ExpectedTest, ExpectedOrElseWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else(
      [](error) -> expected<int, error> { return expected<int, error>(99); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 99);
}

TEST(ExpectedTest, ExpectedTransformWithValue) {
  expected<int, error> e(5);
  auto result = e.transform([](int v) { return v * 3; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 15);
}

TEST(ExpectedTest, ExpectedTransformWithError) {
  expected<int, error> e{unexpected<error>(error::queue_empty)};
  auto result = e.transform([](int v) { return v * 3; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

TEST(ExpectedTest, ExpectedTransformErrorWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 42);
}

TEST(ExpectedTest, ExpectedTransformErrorWithValue) {
  expected<int, error> e(5);
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST(ExpectedVoidTest, DefaultSuccess) {
  expected<void, error> e;
  EXPECT_TRUE(e.has_value());
  EXPECT_TRUE(static_cast<bool>(e));
}

TEST(ExpectedVoidTest, WithError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::timeout);
}

TEST(ExpectedVoidTest, UnexpectConstruction) {
  expected<void, error> e(unexpect, error::out_of_memory);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST(ExpectedVoidTest, CopyConstruction) {
  expected<void, error> e1;
  expected<void, error> e2(e1);
  EXPECT_TRUE(e2.has_value());

  expected<void, error> e3{unexpected<error>(error::queue_full)};
  expected<void, error> e4(e3);
  EXPECT_FALSE(e4.has_value());
  EXPECT_EQ(e4.error(), error::queue_full);
}

TEST(ExpectedVoidTest, AndThenWithValue) {
  expected<void, error> e;
  auto result = e.and_then(
      []() -> expected<int, error> { return expected<int, error>(42); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(ExpectedVoidTest, AndThenWithError) {
  expected<void, error> e{unexpected<error>(error::would_block)};
  auto result = e.and_then(
      []() -> expected<int, error> { return expected<int, error>(42); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST(ExpectedVoidTest, TransformWithValue) {
  expected<void, error> e;
  auto result = e.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(ExpectedVoidTest, TransformError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error) -> int { return 99; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 99);
}

TEST(ExpectedVoidTest, OrElseWithValue) {
  expected<void, error> e;
  auto result = e.or_else(
      [](error) -> expected<void, error> { return expected<void, error>(); });
  EXPECT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, OrElseWithError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else(
      [](error) -> expected<void, error> { return expected<void, error>(); });
  EXPECT_TRUE(result.has_value());
}

// =============================================================================
// Wrong-state accessor hardening (issue #337 / v3.2.1 hotfix C2)
//
// In release builds configASSERT() is a no-op, so calling value() on an error
// state, or error() on a value state, would otherwise return a punned reference
// into uninitialized storage (UB). The accessors now invoke __builtin_trap()
// (or std::terminate()) on the wrong-state path. These EXPECT_DEATH checks
// pin that contract: misuse must crash loudly, never silently return garbage.
// =============================================================================

TEST(ExpectedDeathTest, ValueOnErrorStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        expected<int, error> e(unexpect, error::timeout);
        (void)e.value();
      },
      "");
}

TEST(ExpectedDeathTest, ConstValueOnErrorStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        const expected<int, error> e(unexpect, error::timeout);
        (void)e.value();
      },
      "");
}

TEST(ExpectedDeathTest, RvalueValueOnErrorStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        (void)expected<int, error>(unexpect, error::timeout).value();
      },
      "");
}

TEST(ExpectedDeathTest, ErrorOnValueStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        expected<int, error> e(42);
        (void)e.error();
      },
      "");
}

TEST(ExpectedDeathTest, ConstErrorOnValueStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        const expected<int, error> e(42);
        (void)e.error();
      },
      "");
}

TEST(ExpectedDeathTest, DereferenceOnErrorStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        expected<int, error> e(unexpect, error::timeout);
        (void)*e;
      },
      "");
}

TEST(ExpectedDeathTest, ArrowOnErrorStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        expected<std::string, error> e(unexpect, error::timeout);
        (void)e->size();
      },
      "");
}

TEST(ExpectedDeathTest, VoidErrorOnValueStateTraps) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(
      {
        expected<void, error> e;
        (void)e.error();
      },
      "");
}

// Sanity: the happy paths must keep working unchanged.
TEST(ExpectedDeathTest, HappyPathValueStillWorks) {
  expected<int, error> e(123);
  EXPECT_EQ(e.value(), 123);
  EXPECT_EQ(*e, 123);
}

TEST(ExpectedDeathTest, HappyPathErrorStillWorks) {
  expected<int, error> e(unexpect, error::queue_full);
  EXPECT_EQ(e.error(), error::queue_full);
}

TEST(ExpectedDeathTest, VoidHappyPathErrorStillWorks) {
  expected<void, error> e(unexpect, error::buffer_empty);
  EXPECT_EQ(e.error(), error::buffer_empty);
}

} // namespace
#include <freertos_expected.hpp>

#include <gtest/gtest.h>
#include <string>

namespace {

using freertos::error;
using freertos::expected;
using freertos::unexpect;
using freertos::unexpected;

// ================================================================
// expected<T,E> branch coverage
// ================================================================

class ExpectedCovTest : public ::testing::Test {};

// ---------- Destructor: value path and error path (line 149) ----------

TEST_F(ExpectedCovTest, DestructorValuePathInt) {
  { expected<int, error> e(42); }
}

TEST_F(ExpectedCovTest, DestructorErrorPathInt) {
  { expected<int, error> e{unexpected<error>(error::timeout)}; }
}

TEST_F(ExpectedCovTest, DestructorValuePathString) {
  { expected<std::string, error> e(std::string("hello")); }
}

TEST_F(ExpectedCovTest, DestructorErrorPathString) {
  { expected<std::string, error> e{unexpected<error>(error::out_of_memory)}; }
}

// ---------- Copy constructor: value and error paths (line 156-163) ----------

TEST_F(ExpectedCovTest, CopyConstructorValue) {
  expected<int, error> src(10);
  expected<int, error> dst(src);
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 10);
}

TEST_F(ExpectedCovTest, CopyConstructorError) {
  expected<int, error> src{unexpected<error>(error::timeout)};
  expected<int, error> dst(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::timeout);
}

TEST_F(ExpectedCovTest, CopyConstructorStringValue) {
  expected<std::string, error> src(std::string("abc"));
  expected<std::string, error> dst(src);
  EXPECT_EQ(dst.value(), "abc");
}

TEST_F(ExpectedCovTest, CopyConstructorStringError) {
  expected<std::string, error> src{unexpected<error>(error::invalid_handle)};
  expected<std::string, error> dst(src);
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- Move constructor: value and error paths (line 165-173) ----------

TEST_F(ExpectedCovTest, MoveConstructorValue) {
  expected<int, error> src(20);
  expected<int, error> dst(std::move(src));
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 20);
}

TEST_F(ExpectedCovTest, MoveConstructorError) {
  expected<int, error> src{unexpected<error>(error::queue_full)};
  expected<int, error> dst(std::move(src));
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_full);
}

TEST_F(ExpectedCovTest, MoveConstructorStringValue) {
  expected<std::string, error> src(std::string("move_me"));
  expected<std::string, error> dst(std::move(src));
  EXPECT_EQ(dst.value(), "move_me");
}

TEST_F(ExpectedCovTest, MoveConstructorStringError) {
  expected<std::string, error> src{unexpected<error>(error::would_block)};
  expected<std::string, error> dst(std::move(src));
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- Copy assignment: self, value-to-value, value-to-error, error-to-value, error-to-error (line 175-187) ----------

TEST_F(ExpectedCovTest, CopyAssignmentSelfValue) {
  expected<int, error> e(1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
  EXPECT_EQ(e.value(), 1);
}

TEST_F(ExpectedCovTest, CopyAssignmentSelfError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedCovTest, CopyAssignmentValueToValue) {
  expected<int, error> src(5);
  expected<int, error> dst(10);
  dst = src;
  EXPECT_EQ(dst.value(), 5);
}

TEST_F(ExpectedCovTest, CopyAssignmentValueToError) {
  expected<int, error> src(99);
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 99);
}

TEST_F(ExpectedCovTest, CopyAssignmentErrorToValue) {
  expected<int, error> src{unexpected<error>(error::queue_empty)};
  expected<int, error> dst(42);
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedCovTest, CopyAssignmentErrorToError) {
  expected<int, error> src{unexpected<error>(error::would_block)};
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- Move assignment: self, value-to-value, value-to-error, error-to-value, error-to-error (line 189-201) ----------

TEST_F(ExpectedCovTest, MoveAssignmentSelfValue) {
  expected<int, error> e(1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedCovTest, MoveAssignmentSelfError) {
  expected<int, error> e{unexpected<error>(error::queue_full)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedCovTest, MoveAssignmentValueToValue) {
  expected<int, error> src(7);
  expected<int, error> dst(8);
  dst = std::move(src);
  EXPECT_EQ(dst.value(), 7);
}

TEST_F(ExpectedCovTest, MoveAssignmentValueToError) {
  expected<int, error> src(77);
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 77);
}

TEST_F(ExpectedCovTest, MoveAssignmentErrorToValue) {
  expected<int, error> src{unexpected<error>(error::would_block)};
  expected<int, error> dst(42);
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedCovTest, MoveAssignmentErrorToError) {
  expected<int, error> src{unexpected<error>(error::invalid_handle)};
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

TEST_F(ExpectedCovTest, MoveAssignmentStringValueToError) {
  expected<std::string, error> src(std::string("moved"));
  expected<std::string, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.value(), "moved");
}

TEST_F(ExpectedCovTest, MoveAssignmentStringErrorToValue) {
  expected<std::string, error> src{unexpected<error>(error::would_block)};
  expected<std::string, error> dst(std::string("original"));
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- has_value() / operator bool ----------

TEST_F(ExpectedCovTest, HasValueTrue) {
  expected<int, error> e(42);
  EXPECT_TRUE(e.has_value());
  EXPECT_TRUE(static_cast<bool>(e));
}

TEST_F(ExpectedCovTest, HasValueFalse) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e.has_value());
  EXPECT_FALSE(static_cast<bool>(e));
}

// ---------- value() lvalue&, const&, &&, const&& ----------

TEST_F(ExpectedCovTest, ValueLvalue) {
  expected<int, error> e(42);
  int &ref = e.value();
  EXPECT_EQ(ref, 42);
}

TEST_F(ExpectedCovTest, ValueConstLvalue) {
  const expected<int, error> e(42);
  const int &ref = e.value();
  EXPECT_EQ(ref, 42);
}

TEST_F(ExpectedCovTest, ValueRvalue) {
  expected<int, error> e(42);
  int val = std::move(e).value();
  EXPECT_EQ(val, 42);
}

TEST_F(ExpectedCovTest, ValueConstRvalue) {
  const expected<int, error> e(42);
  const int val = std::move(e).value();
  EXPECT_EQ(val, 42);
}

// ---------- error() lvalue&, const&, &&, const&& ----------

TEST_F(ExpectedCovTest, ErrorLvalue) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedCovTest, ErrorConstLvalue) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  const error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedCovTest, ErrorRvalue) {
  expected<int, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

TEST_F(ExpectedCovTest, ErrorConstRvalue) {
  const expected<int, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

// ---------- operator* lvalue& and const& ----------

TEST_F(ExpectedCovTest, OperatorDerefLvalue) {
  expected<int, error> e(55);
  EXPECT_EQ(*e, 55);
}

TEST_F(ExpectedCovTest, OperatorDerefConstLvalue) {
  const expected<int, error> e(55);
  EXPECT_EQ(*e, 55);
}

TEST_F(ExpectedCovTest, OperatorDerefStringLvalue) {
  expected<std::string, error> e(std::string("test"));
  EXPECT_EQ(*e, "test");
}

TEST_F(ExpectedCovTest, OperatorDerefStringConstLvalue) {
  const expected<std::string, error> e(std::string("test"));
  EXPECT_EQ(*e, "test");
}

// ---------- operator-> non-const and const ----------

TEST_F(ExpectedCovTest, OperatorArrowNonConst) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_EQ(e->size(), 5u);
}

TEST_F(ExpectedCovTest, OperatorArrowConst) {
  const expected<std::string, error> e(std::string("hello"));
  EXPECT_EQ(e->size(), 5u);
}

// ---------- value_or const& with value and with error (line 267-272) ----------

TEST_F(ExpectedCovTest, ValueOrConstRefWithValue) {
  const expected<int, error> e(42);
  EXPECT_EQ(e.value_or(0), 42);
}

TEST_F(ExpectedCovTest, ValueOrConstRefWithError) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(e.value_or(99), 99);
}

TEST_F(ExpectedCovTest, ValueOrConstRefStringWithValue) {
  const expected<std::string, error> e(std::string("present"));
  EXPECT_EQ(e.value_or(std::string("default")), "present");
}

TEST_F(ExpectedCovTest, ValueOrConstRefStringWithError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(e.value_or(std::string("default")), "default");
}

// ---------- value_or && with value and with error (line 274-280) ----------

TEST_F(ExpectedCovTest, ValueOrRvalueWithValue) {
  expected<int, error> e(42);
  EXPECT_EQ(std::move(e).value_or(0), 42);
}

TEST_F(ExpectedCovTest, ValueOrRvalueWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(std::move(e).value_or(99), 99);
}

TEST_F(ExpectedCovTest, ValueOrRvalueStringWithValue) {
  expected<std::string, error> e(std::string("present"));
  EXPECT_EQ(std::move(e).value_or(std::string("default")), "present");
}

TEST_F(ExpectedCovTest, ValueOrRvalueStringWithError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(std::move(e).value_or(std::string("default")), "default");
}

// ---------- and_then lvalue& on VALUE (line 284) ----------

TEST_F(ExpectedCovTest, AndThenLvalueOnValue) {
  expected<int, error> e(10);
  auto result = e.and_then([](int &v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 20);
}

TEST_F(ExpectedCovTest, AndThenLvalueOnError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](int &v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- and_then const& on value and error (line 294) ----------

TEST_F(ExpectedCovTest, AndThenConstLvalueOnValue) {
  const expected<int, error> e(10);
  auto result = e.and_then([](const int &v) -> expected<int, error> {
    return expected<int, error>(v * 3);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 30);
}

// ---------- and_then with string type for additional branch coverage ----------

TEST_F(ExpectedCovTest, AndThenLvalueStringOnValue) {
  expected<std::string, error> e(std::string("abc"));
  auto result = e.and_then([](std::string &s) -> expected<std::string, error> {
    return expected<std::string, error>(s + "def");
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abcdef");
}

TEST_F(ExpectedCovTest, AndThenLvalueStringOnError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](std::string &s) -> expected<std::string, error> {
    return expected<std::string, error>(s + "def");
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedCovTest, AndThenConstLvalueStringOnValue) {
  const expected<std::string, error> e(std::string("abc"));
  auto result = e.and_then([](const std::string &s) -> expected<std::string, error> {
    return expected<std::string, error>(s + "123");
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc123");
}

TEST_F(ExpectedCovTest, AndThenConstLvalueStringOnError) {
  const expected<std::string, error> e{unexpected<error>(error::would_block)};
  auto result = e.and_then([](const std::string &s) -> expected<std::string, error> {
    return expected<std::string, error>(s + "123");
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- or_else lvalue& on value and on error (line 303) ----------

TEST_F(ExpectedCovTest, OrElseLvalueOnValue) {
  expected<int, error> e(5);
  auto result = e.or_else([](error &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedCovTest, OrElseLvalueOnError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &err) -> expected<int, error> {
    return expected<int, error>(100);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 100);
}

TEST_F(ExpectedCovTest, OrElseLvalueOnErrorReturnsError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &err) -> expected<int, error> {
    return expected<int, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- or_else const& on value and on error (line 312) ----------

TEST_F(ExpectedCovTest, OrElseConstLvalueOnValue) {
  const expected<int, error> e(5);
  auto result = e.or_else([](const error &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedCovTest, OrElseConstLvalueOnError) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 99);
}

// ---------- or_else with string type ----------

TEST_F(ExpectedCovTest, OrElseLvalueStringOnValue) {
  expected<std::string, error> e(std::string("keep"));
  auto result = e.or_else([](error &) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("replaced"));
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "keep");
}

TEST_F(ExpectedCovTest, OrElseLvalueStringOnError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("recovered"));
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "recovered");
}

// ---------- transform lvalue& on value and on error (line 323) ----------

TEST_F(ExpectedCovTest, TransformLvalueOnValue) {
  expected<int, error> e(7);
  auto result = e.transform([](int &v) { return v * 3; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 21);
}

TEST_F(ExpectedCovTest, TransformLvalueOnError) {
  expected<int, error> e{unexpected<error>(error::queue_empty)};
  auto result = e.transform([](int &v) { return v * 3; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

// ---------- transform const& on value and on error (line 333) ----------

TEST_F(ExpectedCovTest, TransformConstLvalueOnValue) {
  const expected<int, error> e(7);
  auto result = e.transform([](const int &v) { return v + 1; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 8);
}

TEST_F(ExpectedCovTest, TransformConstLvalueOnError) {
  const expected<int, error> e{unexpected<error>(error::out_of_memory)};
  auto result = e.transform([](const int &v) { return v + 1; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ---------- transform with string type ----------

TEST_F(ExpectedCovTest, TransformLvalueStringOnValue) {
  expected<std::string, error> e(std::string("abc"));
  auto result = e.transform([](std::string &s) -> size_t { return s.size(); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3u);
}

TEST_F(ExpectedCovTest, TransformLvalueStringOnError) {
  expected<std::string, error> e{unexpected<error>(error::invalid_handle)};
  auto result = e.transform([](std::string &s) -> size_t { return s.size(); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::invalid_handle);
}

TEST_F(ExpectedCovTest, TransformConstLvalueStringOnValue) {
  const expected<std::string, error> e(std::string("hello"));
  auto result = e.transform([](const std::string &s) -> size_t { return s.size(); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5u);
}

TEST_F(ExpectedCovTest, TransformConstLvalueStringOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform([](const std::string &s) -> size_t { return s.size(); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- transform_error lvalue& on value and on error (line 345) ----------

TEST_F(ExpectedCovTest, TransformErrorLvalueOnValue) {
  expected<int, error> e(5);
  auto result = e.transform_error([](error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedCovTest, TransformErrorLvalueOnError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error &err) -> int { return static_cast<int>(err); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 1);
}

// ---------- transform_error const& on value and on error (line 355) ----------

TEST_F(ExpectedCovTest, TransformErrorConstLvalueOnValue) {
  const expected<int, error> e(5);
  auto result = e.transform_error([](const error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedCovTest, TransformErrorConstLvalueOnError) {
  const expected<int, error> e{unexpected<error>(error::invalid_handle)};
  auto result = e.transform_error([](const error &err) -> int { return static_cast<int>(err); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 6);
}

// ---------- transform_error with string type ----------

TEST_F(ExpectedCovTest, TransformErrorLvalueStringOnValue) {
  expected<std::string, error> e(std::string("abc"));
  auto result = e.transform_error([](error &) -> int { return 99; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc");
}

TEST_F(ExpectedCovTest, TransformErrorLvalueStringOnError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error &) -> int { return 50; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 50);
}

TEST_F(ExpectedCovTest, TransformErrorConstLvalueStringOnValue) {
  const expected<std::string, error> e(std::string("abc"));
  auto result = e.transform_error([](const error &) -> int { return 99; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc");
}

TEST_F(ExpectedCovTest, TransformErrorConstLvalueStringOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](const error &) -> int { return 50; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 50);
}

// ---------- Comparison operators: expected == expected (line 364-374) ----------

TEST_F(ExpectedCovTest, EqualBothValue) {
  expected<int, error> a(42);
  expected<int, error> b(42);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedCovTest, EqualBothValueDifferent) {
  expected<int, error> a(42);
  expected<int, error> b(99);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedCovTest, EqualBothError) {
  expected<int, error> a{unexpected<error>(error::timeout)};
  expected<int, error> b{unexpected<error>(error::timeout)};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedCovTest, EqualBothErrorDifferent) {
  expected<int, error> a{unexpected<error>(error::timeout)};
  expected<int, error> b{unexpected<error>(error::queue_full)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedCovTest, EqualValueVsError) {
  expected<int, error> a(42);
  expected<int, error> b{unexpected<error>(error::timeout)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedCovTest, EqualErrorVsValue) {
  expected<int, error> a{unexpected<error>(error::timeout)};
  expected<int, error> b(42);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

// ---------- Comparison: expected == T (line 380-383) ----------

TEST_F(ExpectedCovTest, EqualsTWithValue) {
  expected<int, error> e(42);
  EXPECT_TRUE(e == 42);
  EXPECT_FALSE(e == 99);
}

TEST_F(ExpectedCovTest, EqualsTWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == 42);
}

// ---------- Comparison: expected == unexpected<E> (line 385-388) ----------

TEST_F(ExpectedCovTest, EqualsUnexpectedWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_TRUE(e == unexpected<error>(error::timeout));
  EXPECT_FALSE(e == unexpected<error>(error::queue_full));
}

TEST_F(ExpectedCovTest, EqualsUnexpectedWithValue) {
  expected<int, error> e(42);
  EXPECT_FALSE(e == unexpected<error>(error::timeout));
}

// ================================================================
// expected<void, E> branch coverage
// ================================================================

class ExpectedVoidCovTest : public ::testing::Test {};

// ---------- void destructor: value and error paths (line 424-428) ----------

TEST_F(ExpectedVoidCovTest, DestructorValuePath) {
  { expected<void, error> e; }
}

TEST_F(ExpectedVoidCovTest, DestructorErrorPath) {
  { expected<void, error> e{unexpected<error>(error::timeout)}; }
}

// ---------- void copy constructor: value and error paths (line 430-435) ----------

TEST_F(ExpectedVoidCovTest, CopyConstructorValue) {
  expected<void, error> src;
  expected<void, error> dst(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, CopyConstructorError) {
  expected<void, error> src{unexpected<error>(error::timeout)};
  expected<void, error> dst(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::timeout);
}

// ---------- void move constructor: value and error paths (line 437-443) ----------

TEST_F(ExpectedVoidCovTest, MoveConstructorValue) {
  expected<void, error> src;
  expected<void, error> dst(std::move(src));
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, MoveConstructorError) {
  expected<void, error> src{unexpected<error>(error::queue_empty)};
  expected<void, error> dst(std::move(src));
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

// ---------- void copy assignment (line 445-457) ----------

TEST_F(ExpectedVoidCovTest, CopyAssignmentSelfValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoidCovTest, CopyAssignmentSelfError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedVoidCovTest, CopyAssignmentValueToValue) {
  expected<void, error> src;
  expected<void, error> dst;
  dst = src;
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, CopyAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, CopyAssignmentErrorToValue) {
  expected<void, error> src{unexpected<error>(error::queue_empty)};
  expected<void, error> dst;
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedVoidCovTest, CopyAssignmentErrorToError) {
  expected<void, error> src{unexpected<error>(error::would_block)};
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- void move assignment (line 459-471) ----------

TEST_F(ExpectedVoidCovTest, MoveAssignmentSelfValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoidCovTest, MoveAssignmentSelfError) {
  expected<void, error> e{unexpected<error>(error::queue_full)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedVoidCovTest, MoveAssignmentValueToValue) {
  expected<void, error> src;
  expected<void, error> dst;
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, MoveAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidCovTest, MoveAssignmentErrorToValue) {
  expected<void, error> src{unexpected<error>(error::would_block)};
  expected<void, error> dst;
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedVoidCovTest, MoveAssignmentErrorToError) {
  expected<void, error> src{unexpected<error>(error::invalid_handle)};
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- void has_value / operator bool ----------

TEST_F(ExpectedVoidCovTest, HasValueTrue) {
  expected<void, error> e;
  EXPECT_TRUE(e.has_value());
  EXPECT_TRUE(static_cast<bool>(e));
}

TEST_F(ExpectedVoidCovTest, HasValueFalse) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e.has_value());
  EXPECT_FALSE(static_cast<bool>(e));
}

// ---------- void error() lvalue, const&, &&, const&& ----------

TEST_F(ExpectedVoidCovTest, ErrorLvalue) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedVoidCovTest, ErrorConstLvalue) {
  const expected<void, error> e{unexpected<error>(error::timeout)};
  const error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedVoidCovTest, ErrorRvalue) {
  expected<void, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

TEST_F(ExpectedVoidCovTest, ErrorConstRvalue) {
  const expected<void, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

// ---------- void and_then lvalue& on value and on error (line 496-502) ----------

TEST_F(ExpectedVoidCovTest, AndThenLvalueOnValue) {
  expected<void, error> e;
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoidCovTest, AndThenLvalueOnError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- void and_then const& on value and on error (line 504-511) ----------

TEST_F(ExpectedVoidCovTest, AndThenConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoidCovTest, AndThenConstLvalueOnError) {
  const expected<void, error> e{unexpected<error>(error::would_block)};
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- void or_else lvalue& on value and error (line 513-519) ----------

TEST_F(ExpectedVoidCovTest, OrElseLvalueOnValue) {
  expected<void, error> e;
  auto result = e.or_else([](error &) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, OrElseLvalueOnError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, OrElseLvalueOnErrorReturnsError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &err) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- void or_else const& on value and error (line 521-528) ----------

TEST_F(ExpectedVoidCovTest, OrElseConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, OrElseConstLvalueOnError) {
  const expected<void, error> e{unexpected<error>(error::queue_empty)};
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, OrElseConstLvalueOnErrorReturnsError) {
  const expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- void transform lvalue& on value and error (line 530-537) ----------

TEST_F(ExpectedVoidCovTest, TransformLvalueOnValue) {
  expected<void, error> e;
  auto result = e.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoidCovTest, TransformLvalueOnError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- void transform const& on value and error (line 539-546) ----------

TEST_F(ExpectedVoidCovTest, TransformConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoidCovTest, TransformConstLvalueOnError) {
  const expected<void, error> e{unexpected<error>(error::out_of_memory)};
  auto result = e.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ---------- void transform_error lvalue& on value and error (line 548-556) ----------

TEST_F(ExpectedVoidCovTest, TransformErrorLvalueOnValue) {
  expected<void, error> e;
  auto result = e.transform_error([](error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, TransformErrorLvalueOnError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error &err) -> int { return static_cast<int>(err); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 1);
}

// ---------- void transform_error const& on value and error (line 558-566) ----------

TEST_F(ExpectedVoidCovTest, TransformErrorConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.transform_error([](const error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoidCovTest, TransformErrorConstLvalueOnError) {
  const expected<void, error> e{unexpected<error>(error::invalid_handle)};
  auto result = e.transform_error([](const error &err) -> int { return static_cast<int>(err); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 6);
}

// ---------- void comparison operators (line 568-586) ----------

TEST_F(ExpectedVoidCovTest, EqualBothValue) {
  expected<void, error> a;
  expected<void, error> b;
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedVoidCovTest, EqualBothErrorSame) {
  expected<void, error> a{unexpected<error>(error::timeout)};
  expected<void, error> b{unexpected<error>(error::timeout)};
  EXPECT_TRUE(a == b);
}

TEST_F(ExpectedVoidCovTest, EqualBothErrorDifferent) {
  expected<void, error> a{unexpected<error>(error::timeout)};
  expected<void, error> b{unexpected<error>(error::queue_full)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedVoidCovTest, EqualValueVsError) {
  expected<void, error> a;
  expected<void, error> b{unexpected<error>(error::timeout)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedVoidCovTest, EqualErrorVsValue) {
  expected<void, error> a{unexpected<error>(error::timeout)};
  expected<void, error> b;
  EXPECT_FALSE(a == b);
}

// ---------- void operator== unexpected (line 583-586) ----------

TEST_F(ExpectedVoidCovTest, EqualsUnexpectedWithError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  EXPECT_TRUE(e == unexpected<error>(error::timeout));
  EXPECT_FALSE(e == unexpected<error>(error::queue_full));
}

TEST_F(ExpectedVoidCovTest, EqualsUnexpectedWithValue) {
  expected<void, error> e;
  EXPECT_FALSE(e == unexpected<error>(error::timeout));
}

// ================================================================
// unexpected<E> branch coverage
// ================================================================

class UnexpectedCovTest : public ::testing::Test {};

TEST_F(UnexpectedCovTest, ValueLvalue) {
  unexpected<error> u(error::timeout);
  error &ref = u.value();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(UnexpectedCovTest, ValueConstLvalue) {
  const unexpected<error> u(error::timeout);
  const error &ref = u.value();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(UnexpectedCovTest, ValueRvalue) {
  unexpected<error> u(error::queue_full);
  error val = std::move(u).value();
  EXPECT_EQ(val, error::queue_full);
}

TEST_F(UnexpectedCovTest, ValueConstRvalue) {
  const unexpected<error> u(error::would_block);
  error val = std::move(u).value();
  EXPECT_EQ(val, error::would_block);
}

TEST_F(UnexpectedCovTest, Equality) {
  unexpected<error> u1(error::timeout);
  unexpected<error> u2(error::timeout);
  unexpected<error> u3(error::queue_full);
  EXPECT_TRUE(u1 == u2);
  EXPECT_FALSE(u1 == u3);
}

TEST_F(UnexpectedCovTest, Inequality) {
  unexpected<error> u1(error::timeout);
  unexpected<error> u2(error::queue_full);
  EXPECT_TRUE(u1 != u2);
  EXPECT_FALSE(u1 != unexpected<error>(error::timeout));
}

// ================================================================
// Additional edge cases: construct from lvalue T and rvalue T
// ================================================================

TEST_F(ExpectedCovTest, ConstructFromLvalueT) {
  int v = 42;
  expected<int, error> e(v);
  EXPECT_EQ(e.value(), 42);
}

TEST_F(ExpectedCovTest, ConstructFromRvalueT) {
  expected<int, error> e(42);
  EXPECT_EQ(e.value(), 42);
}

TEST_F(ExpectedCovTest, ConstructFromStringLvalue) {
  std::string s = "hello";
  expected<std::string, error> e(s);
  EXPECT_EQ(e.value(), "hello");
}

TEST_F(ExpectedCovTest, ConstructFromStringRvalue) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_EQ(e.value(), "hello");
}

// ================================================================
// unexpect_t construction: lvalue and rvalue error
// ================================================================

TEST_F(ExpectedCovTest, ConstructUnexpectLvalue) {
  error err = error::invalid_parameter;
  expected<int, error> e(unexpect, err);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_parameter);
}

TEST_F(ExpectedCovTest, ConstructUnexpectRvalue) {
  expected<int, error> e(unexpect, error::invalid_handle);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_handle);
}

TEST_F(ExpectedCovTest, ConstructUnexpectStringLvalue) {
  error err = error::timeout;
  expected<std::string, error> e(unexpect, err);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedCovTest, ConstructUnexpectStringRvalue) {
  expected<std::string, error> e(unexpect, error::queue_full);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::queue_full);
}

// ================================================================
// Void expected: construct from unexpected lvalue and rvalue
// ================================================================

TEST_F(ExpectedVoidCovTest, ConstructFromUnexpectedLvalue) {
  unexpected<error> u(error::would_block);
  expected<void, error> e(u);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::would_block);
}

TEST_F(ExpectedVoidCovTest, ConstructFromUnexpectedRvalue) {
  auto u = unexpected<error>(error::out_of_memory);
  expected<void, error> e(std::move(u));
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST_F(ExpectedVoidCovTest, ConstructFromUnexpectLvalue) {
  error err = error::invalid_parameter;
  expected<void, error> e(unexpect, err);
  EXPECT_EQ(e.error(), error::invalid_parameter);
}

TEST_F(ExpectedVoidCovTest, ConstructFromUnexpectRvalue) {
  expected<void, error> e(unexpect, error::invalid_handle);
  EXPECT_EQ(e.error(), error::invalid_handle);
}

} // namespace
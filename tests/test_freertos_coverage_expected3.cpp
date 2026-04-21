#include <freertos_expected.hpp>

#include <gtest/gtest.h>
#include <string>

namespace {

using freertos::error;
using freertos::expected;
using freertos::unexpect;
using freertos::unexpected;

// ================================================================
// expected<void, error> — monadic ops returning expected<void, error>
// These exercise specific template instantiations that differ from
// the expected<int,error> return types used in expected2 tests.
// ================================================================

class ExpectedVoid3Test : public ::testing::Test {};

// ---------- void and_then returning expected<void, error> (L497, L506) ----------

TEST_F(ExpectedVoid3Test, AndThenLvalueOnValueReturnsVoid) {
  expected<void, error> e;
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, AndThenLvalueOnErrorReturnsVoid) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedVoid3Test, AndThenLvalueOnValueReturnsVoidError) {
  expected<void, error> e;
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::out_of_memory)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

TEST_F(ExpectedVoid3Test, AndThenLvalueOnErrorReturnsVoidError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::out_of_memory)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedVoid3Test, AndThenConstLvalueOnValueReturnsVoid) {
  const expected<void, error> e;
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, AndThenConstLvalueOnErrorReturnsVoid) {
  const expected<void, error> e{unexpected<error>(error::would_block)};
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(ExpectedVoid3Test, AndThenConstLvalueOnValueReturnsVoidError) {
  const expected<void, error> e;
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::queue_full)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

TEST_F(ExpectedVoid3Test, AndThenConstLvalueOnErrorReturnsVoidError) {
  const expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([]() -> expected<void, error> {
    return expected<void, error>{unexpected<error>(error::queue_full)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- void or_else returning expected<void, error> (L515, L524) ----------

TEST_F(ExpectedVoid3Test, OrElseLvalueOnValueReturnsVoid) {
  expected<void, error> e;
  auto result = e.or_else([](error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, OrElseLvalueOnErrorReturnsVoidValue) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, OrElseLvalueOnErrorReturnsVoidError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](error &err) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(err)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedVoid3Test, OrElseConstLvalueOnValueReturnsVoid) {
  const expected<void, error> e;
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, OrElseConstLvalueOnErrorReturnsVoidValue) {
  const expected<void, error> e{unexpected<error>(error::queue_empty)};
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, OrElseConstLvalueOnErrorReturnsVoidError) {
  const expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &err) -> expected<void, error> {
    return expected<void, error>{unexpected<error>(err)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- void transform returning expected<void, error> (L532, L541) ----------
// A transform lambda returning void would create expected<void, error>,
// but expected<void,E>(f()) doesn't compile when f() returns void.
// Instead, test transform returning expected<int, error> (already in expected2)
// and also test with error state to fully cover branches.

// ---------- void transform_error returning expected<void, error> (L551, L561) ----------

TEST_F(ExpectedVoid3Test, TransformErrorLvalueOnValueReturnsError) {
  expected<void, error> e;
  auto result = e.transform_error([](error &) -> error { return error::ok; });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, TransformErrorLvalueOnErrorReturnsError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error &) -> error { return error::would_block; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(ExpectedVoid3Test, TransformErrorConstLvalueOnValueReturnsError) {
  const expected<void, error> e;
  auto result = e.transform_error([](const error &) -> error { return error::ok; });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedVoid3Test, TransformErrorConstLvalueOnErrorReturnsError) {
  const expected<void, error> e{unexpected<error>(error::invalid_handle)};
  auto result = e.transform_error([](const error &) -> error { return error::would_block; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- void and_then returning expected<int, error> (completing both branches already in expected2) ----------
// Already covered in expected2 but adding explicit error-propagation-from-f tests:

TEST_F(ExpectedVoid3Test, AndThenLvalueOnValueReturnsIntError) {
  expected<void, error> e;
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>{unexpected<error>(error::out_of_memory)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

TEST_F(ExpectedVoid3Test, AndThenConstLvalueOnValueReturnsIntError) {
  const expected<void, error> e;
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>{unexpected<error>(error::queue_full)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_full);
}

// ---------- void operator== with different has_value states (L568-577) ----------

TEST_F(ExpectedVoid3Test, EqualValueVsErrorDifferent) {
  expected<void, error> a;
  expected<void, error> b{unexpected<error>(error::timeout)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
}

TEST_F(ExpectedVoid3Test, EqualBothValue) {
  expected<void, error> a;
  expected<void, error> b;
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedVoid3Test, EqualBothErrorSame) {
  expected<void, error> a{unexpected<error>(error::timeout)};
  expected<void, error> b{unexpected<error>(error::timeout)};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedVoid3Test, EqualBothErrorDifferent) {
  expected<void, error> a{unexpected<error>(error::timeout)};
  expected<void, error> b{unexpected<error>(error::queue_full)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedVoid3Test, EqualsUnexpectedWithValue) {
  expected<void, error> e;
  EXPECT_FALSE(e == unexpected<error>(error::timeout));
}

TEST_F(ExpectedVoid3Test, EqualsUnexpectedWithErrorSame) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  EXPECT_TRUE(e == unexpected<error>(error::timeout));
}

TEST_F(ExpectedVoid3Test, EqualsUnexpectedWithErrorDifferent) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == unexpected<error>(error::queue_full));
}

// ================================================================
// expected<int, error> — filling remaining gaps
// ================================================================

class ExpectedInt3Test : public ::testing::Test {};

// ---------- and_then const & on error for expected<int, error> (L294-299) ----------

TEST_F(ExpectedInt3Test, AndThenConstLvalueOnError) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](const int &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- or_else const & returning error for expected<int, error> (L313-318) ----------

TEST_F(ExpectedInt3Test, OrElseConstLvalueOnErrorReturnsError) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &) -> expected<int, error> {
    return expected<int, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- operator=(expected&&) with self-assignment and transitions (L190, L193) ----------

TEST_F(ExpectedInt3Test, MoveAssignmentSelfValue) {
  expected<int, error> e(42);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedInt3Test, MoveAssignmentSelfError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedInt3Test, MoveAssignmentValueToValue) {
  expected<int, error> src(7);
  expected<int, error> dst(8);
  dst = std::move(src);
  EXPECT_EQ(dst.value(), 7);
}

TEST_F(ExpectedInt3Test, MoveAssignmentValueToError) {
  expected<int, error> src(77);
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 77);
}

TEST_F(ExpectedInt3Test, MoveAssignmentErrorToValue) {
  expected<int, error> src{unexpected<error>(error::would_block)};
  expected<int, error> dst(42);
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedInt3Test, MoveAssignmentErrorToError) {
  expected<int, error> src{unexpected<error>(error::invalid_handle)};
  expected<int, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- operator==(expected&&, expected&&) edge cases for L568 ----------

TEST_F(ExpectedInt3Test, EqualDifferentHasValueValueVsError) {
  expected<int, error> a(42);
  expected<int, error> b{unexpected<error>(error::timeout)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
}

TEST_F(ExpectedInt3Test, EqualBothValueSame) {
  expected<int, error> a(42);
  expected<int, error> b(42);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedInt3Test, EqualBothValueDifferent) {
  expected<int, error> a(42);
  expected<int, error> b(99);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedInt3Test, EqualBothErrorSame) {
  expected<int, error> a{unexpected<error>(error::timeout)};
  expected<int, error> b{unexpected<error>(error::timeout)};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedInt3Test, EqualBothErrorDifferent) {
  expected<int, error> a{unexpected<error>(error::timeout)};
  expected<int, error> b{unexpected<error>(error::queue_full)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

// ---------- operator== with T and unexpected (L380-388) ----------

TEST_F(ExpectedInt3Test, EqualsTWithValueSame) {
  expected<int, error> e(42);
  EXPECT_TRUE(e == 42);
}

TEST_F(ExpectedInt3Test, EqualsTWithValueDifferent) {
  expected<int, error> e(42);
  EXPECT_FALSE(e == 99);
}

TEST_F(ExpectedInt3Test, EqualsTWithError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == 42);
}

TEST_F(ExpectedInt3Test, EqualsUnexpectedWithErrorSame) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_TRUE(e == unexpected<error>(error::timeout));
}

TEST_F(ExpectedInt3Test, EqualsUnexpectedWithErrorDifferent) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == unexpected<error>(error::queue_full));
}

TEST_F(ExpectedInt3Test, EqualsUnexpectedWithValue) {
  expected<int, error> e(42);
  EXPECT_FALSE(e == unexpected<error>(error::timeout));
}

// ================================================================
// expected<std::string, error> — filling remaining gaps
// ================================================================

class ExpectedString3Test : public ::testing::Test {};

// ---------- String move assignment all transitions (L190, L193) ----------

TEST_F(ExpectedString3Test, MoveAssignmentSelfValue) {
  expected<std::string, error> e(std::string("hello"));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedString3Test, MoveAssignmentSelfError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedString3Test, MoveAssignmentValueToValue) {
  expected<std::string, error> src(std::string("src"));
  expected<std::string, error> dst(std::string("dst"));
  dst = std::move(src);
  EXPECT_EQ(dst.value(), "src");
}

TEST_F(ExpectedString3Test, MoveAssignmentErrorToError) {
  expected<std::string, error> src{unexpected<error>(error::would_block)};
  expected<std::string, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- String copy assignment all transitions (L175-187) ----------

TEST_F(ExpectedString3Test, CopyAssignmentSelfValue) {
  expected<std::string, error> e(std::string("hello"));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_EQ(e.value(), "hello");
}

TEST_F(ExpectedString3Test, CopyAssignmentSelfError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedString3Test, CopyAssignmentValueToValue) {
  expected<std::string, error> src(std::string("src"));
  expected<std::string, error> dst(std::string("dst"));
  dst = src;
  EXPECT_EQ(dst.value(), "src");
}

TEST_F(ExpectedString3Test, CopyAssignmentValueToError) {
  expected<std::string, error> src(std::string("src"));
  expected<std::string, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), "src");
}

TEST_F(ExpectedString3Test, CopyAssignmentErrorToValue) {
  expected<std::string, error> src{unexpected<error>(error::queue_empty)};
  expected<std::string, error> dst(std::string("dst"));
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedString3Test, CopyAssignmentErrorToError) {
  expected<std::string, error> src{unexpected<error>(error::would_block)};
  expected<std::string, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- String and_then const & on error (L294-299) ----------

TEST_F(ExpectedString3Test, AndThenConstLvalueOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](const std::string &) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("never"));
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- String or_else const & on value and error (L312-318) ----------

TEST_F(ExpectedString3Test, OrElseConstLvalueOnValue) {
  const expected<std::string, error> e(std::string("keep"));
  auto result = e.or_else([](const error &) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("replaced"));
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "keep");
}

TEST_F(ExpectedString3Test, OrElseConstLvalueOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("recovered"));
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "recovered");
}

TEST_F(ExpectedString3Test, OrElseConstLvalueOnErrorReturnsError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.or_else([](const error &) -> expected<std::string, error> {
    return expected<std::string, error>{unexpected<error>(error::would_block)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- String transform const & on value and error (L333-340) ----------

TEST_F(ExpectedString3Test, TransformConstLvalueOnValue) {
  const expected<std::string, error> e(std::string("hello"));
  auto result = e.transform([](const std::string &s) -> int {
    return static_cast<int>(s.size());
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedString3Test, TransformConstLvalueOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform([](const std::string &s) -> int {
    return static_cast<int>(s.size());
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- String transform_error const & on value and error (L355-362) ----------

TEST_F(ExpectedString3Test, TransformErrorConstLvalueOnValue) {
  const expected<std::string, error> e(std::string("abc"));
  auto result = e.transform_error([](const error &) -> int { return 99; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc");
}

TEST_F(ExpectedString3Test, TransformErrorConstLvalueOnError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](const error &err) -> int {
    return static_cast<int>(err);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 1);
}

// ---------- String operator== all comparison combos (L364-374) ----------

TEST_F(ExpectedString3Test, EqualBothValueSame) {
  expected<std::string, error> a(std::string("hello"));
  expected<std::string, error> b(std::string("hello"));
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedString3Test, EqualBothValueDifferent) {
  expected<std::string, error> a(std::string("hello"));
  expected<std::string, error> b(std::string("world"));
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedString3Test, EqualBothErrorSame) {
  expected<std::string, error> a{unexpected<error>(error::timeout)};
  expected<std::string, error> b{unexpected<error>(error::timeout)};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST_F(ExpectedString3Test, EqualBothErrorDifferent) {
  expected<std::string, error> a{unexpected<error>(error::timeout)};
  expected<std::string, error> b{unexpected<error>(error::queue_full)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedString3Test, EqualValueVsError) {
  expected<std::string, error> a(std::string("hello"));
  expected<std::string, error> b{unexpected<error>(error::timeout)};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST_F(ExpectedString3Test, EqualErrorVsValue) {
  expected<std::string, error> a{unexpected<error>(error::timeout)};
  expected<std::string, error> b(std::string("hello"));
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

// ---------- String operator== with T and unexpected ----------

TEST_F(ExpectedString3Test, EqualsTWithValueSame) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_TRUE(e == std::string("hello"));
}

TEST_F(ExpectedString3Test, EqualsTWithValueDifferent) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_FALSE(e == std::string("world"));
}

TEST_F(ExpectedString3Test, EqualsTWithError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == std::string("hello"));
}

TEST_F(ExpectedString3Test, EqualsUnexpectedWithErrorSame) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_TRUE(e == unexpected<error>(error::timeout));
}

TEST_F(ExpectedString3Test, EqualsUnexpectedWithErrorDifferent) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_FALSE(e == unexpected<error>(error::queue_full));
}

TEST_F(ExpectedString3Test, EqualsUnexpectedWithValue) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_FALSE(e == unexpected<error>(error::timeout));
}

// ---------- String value_or const& and && ----------

TEST_F(ExpectedString3Test, ValueOrConstRefWithValue) {
  const expected<std::string, error> e(std::string("present"));
  EXPECT_EQ(e.value_or(std::string("default")), "present");
}

TEST_F(ExpectedString3Test, ValueOrConstRefWithError) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(e.value_or(std::string("default")), "default");
}

TEST_F(ExpectedString3Test, ValueOrRvalueWithValue) {
  expected<std::string, error> e(std::string("present"));
  EXPECT_EQ(std::move(e).value_or(std::string("default")), "present");
}

TEST_F(ExpectedString3Test, ValueOrRvalueWithError) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  EXPECT_EQ(std::move(e).value_or(std::string("default")), "default");
}

// ---------- String error() all ref qualifiers ----------

TEST_F(ExpectedString3Test, ErrorLvalue) {
  expected<std::string, error> e{unexpected<error>(error::timeout)};
  error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedString3Test, ErrorConstLvalue) {
  const expected<std::string, error> e{unexpected<error>(error::timeout)};
  const error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedString3Test, ErrorRvalue) {
  expected<std::string, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

TEST_F(ExpectedString3Test, ErrorConstRvalue) {
  const expected<std::string, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

// ---------- String value() all ref qualifiers ----------

TEST_F(ExpectedString3Test, ValueLvalue) {
  expected<std::string, error> e(std::string("hello"));
  std::string &ref = e.value();
  EXPECT_EQ(ref, "hello");
}

TEST_F(ExpectedString3Test, ValueConstLvalue) {
  const expected<std::string, error> e(std::string("hello"));
  const std::string &ref = e.value();
  EXPECT_EQ(ref, "hello");
}

TEST_F(ExpectedString3Test, ValueRvalue) {
  expected<std::string, error> e(std::string("hello"));
  std::string val = std::move(e).value();
  EXPECT_EQ(val, "hello");
}

TEST_F(ExpectedString3Test, ValueConstRvalue) {
  const expected<std::string, error> e(std::string("hello"));
  std::string val = std::move(e).value();
  EXPECT_EQ(val, "hello");
}

// ================================================================
// expected<void, error> — constructor and destructor edge cases
// ================================================================

TEST_F(ExpectedVoid3Test, DefaultConstructor) {
  expected<void, error> e;
  EXPECT_TRUE(e.has_value());
  EXPECT_TRUE(static_cast<bool>(e));
}

TEST_F(ExpectedVoid3Test, ConstructFromUnexpectedLvalue) {
  unexpected<error> u(error::would_block);
  expected<void, error> e(u);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::would_block);
}

TEST_F(ExpectedVoid3Test, ConstructFromUnexpectedRvalue) {
  auto u = unexpected<error>(error::out_of_memory);
  expected<void, error> e(std::move(u));
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST_F(ExpectedVoid3Test, ConstructFromUnexpectLvalue) {
  error err = error::invalid_parameter;
  expected<void, error> e(unexpect, err);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_parameter);
}

TEST_F(ExpectedVoid3Test, ConstructFromUnexpectRvalue) {
  expected<void, error> e(unexpect, error::invalid_handle);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_handle);
}

// ---------- Void copy/move assignment all transitions ----------

TEST_F(ExpectedVoid3Test, CopyAssignmentSelfValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoid3Test, CopyAssignmentSelfError) {
  expected<void, error> e{unexpected<error>(error::timeout)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedVoid3Test, CopyAssignmentValueToValue) {
  expected<void, error> src;
  expected<void, error> dst;
  dst = src;
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, CopyAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, CopyAssignmentErrorToValue) {
  expected<void, error> src{unexpected<error>(error::queue_empty)};
  expected<void, error> dst;
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedVoid3Test, CopyAssignmentErrorToError) {
  expected<void, error> src{unexpected<error>(error::would_block)};
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = src;
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedVoid3Test, MoveAssignmentSelfValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoid3Test, MoveAssignmentSelfError) {
  expected<void, error> e{unexpected<error>(error::queue_full)};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
}

TEST_F(ExpectedVoid3Test, MoveAssignmentValueToValue) {
  expected<void, error> src;
  expected<void, error> dst;
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, MoveAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, MoveAssignmentErrorToValue) {
  expected<void, error> src{unexpected<error>(error::would_block)};
  expected<void, error> dst;
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedVoid3Test, MoveAssignmentErrorToError) {
  expected<void, error> src{unexpected<error>(error::invalid_handle)};
  expected<void, error> dst{unexpected<error>(error::timeout)};
  dst = std::move(src);
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- Void copy/move constructors ----------

TEST_F(ExpectedVoid3Test, CopyConstructorValue) {
  expected<void, error> src;
  expected<void, error> dst(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, CopyConstructorError) {
  expected<void, error> src{unexpected<error>(error::timeout)};
  expected<void, error> dst(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::timeout);
}

TEST_F(ExpectedVoid3Test, MoveConstructorValue) {
  expected<void, error> src;
  expected<void, error> dst(std::move(src));
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoid3Test, MoveConstructorError) {
  expected<void, error> src{unexpected<error>(error::queue_empty)};
  expected<void, error> dst(std::move(src));
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

// ---------- Void destructor paths ----------

TEST_F(ExpectedVoid3Test, DestructorValuePath) {
  { expected<void, error> e; }
}

TEST_F(ExpectedVoid3Test, DestructorErrorPath) {
  { expected<void, error> e{unexpected<error>(error::timeout)}; }
}

// ---------- Void error() all ref qualifiers ----------

TEST_F(ExpectedVoid3Test, ErrorLvalue) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedVoid3Test, ErrorConstLvalue) {
  const expected<void, error> e{unexpected<error>(error::timeout)};
  const error &ref = e.error();
  EXPECT_EQ(ref, error::timeout);
}

TEST_F(ExpectedVoid3Test, ErrorRvalue) {
  expected<void, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

TEST_F(ExpectedVoid3Test, ErrorConstRvalue) {
  const expected<void, error> e{unexpected<error>(error::queue_full)};
  error val = std::move(e).error();
  EXPECT_EQ(val, error::queue_full);
}

// ================================================================
// expected<int, error> — and_then returning expected<int, error> (self-type)
// ================================================================

TEST_F(ExpectedInt3Test, AndThenLvalueOnValueReturnsSelf) {
  expected<int, error> e(10);
  auto result = e.and_then([](int &v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 20);
}

TEST_F(ExpectedInt3Test, AndThenLvalueOnErrorReturnsSelf) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](int &v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedInt3Test, AndThenConstLvalueOnValueReturnsSelf) {
  const expected<int, error> e(10);
  auto result = e.and_then([](const int &v) -> expected<int, error> {
    return expected<int, error>(v * 3);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 30);
}

TEST_F(ExpectedInt3Test, AndThenConstLvalueOnErrorReturnsSelf) {
  const expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.and_then([](const int &v) -> expected<int, error> {
    return expected<int, error>(v * 3);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ================================================================
// expected<int, error> — transform returning expected<int, error> (self-type)
// ================================================================

TEST_F(ExpectedInt3Test, TransformLvalueOnValueReturnsInt) {
  expected<int, error> e(7);
  auto result = e.transform([](int &v) -> int { return v * 3; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 21);
}

TEST_F(ExpectedInt3Test, TransformLvalueOnErrorReturnsInt) {
  expected<int, error> e{unexpected<error>(error::queue_empty)};
  auto result = e.transform([](int &v) -> int { return v * 3; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

TEST_F(ExpectedInt3Test, TransformConstLvalueOnValueReturnsInt) {
  const expected<int, error> e(7);
  auto result = e.transform([](const int &v) -> int { return v + 1; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 8);
}

TEST_F(ExpectedInt3Test, TransformConstLvalueOnErrorReturnsInt) {
  const expected<int, error> e{unexpected<error>(error::out_of_memory)};
  auto result = e.transform([](const int &v) -> int { return v + 1; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ================================================================
// expected<int, error> — transform_error returning expected<int, error> (self-type)
// ================================================================

TEST_F(ExpectedInt3Test, TransformErrorLvalueOnValueReturnsError) {
  expected<int, error> e(5);
  auto result = e.transform_error([](error &) -> error { return error::ok; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedInt3Test, TransformErrorLvalueOnErrorReturnsError) {
  expected<int, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform_error([](error &err) -> error {
    return err == error::timeout ? error::would_block : err;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(ExpectedInt3Test, TransformErrorConstLvalueOnValueReturnsError) {
  const expected<int, error> e(5);
  auto result = e.transform_error([](const error &) -> error { return error::ok; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(ExpectedInt3Test, TransformErrorConstLvalueOnErrorReturnsError) {
  const expected<int, error> e{unexpected<error>(error::invalid_handle)};
  auto result = e.transform_error([](const error &err) -> error {
    return err == error::invalid_handle ? error::would_block : err;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ================================================================
// expected<void, error> — transform returning expected<int, error>
// (already in expected2 but ensuring int return type coverage)
// ================================================================

TEST_F(ExpectedVoid3Test, TransformLvalueOnValueReturnsInt) {
  expected<void, error> e;
  auto result = e.transform([]() -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoid3Test, TransformLvalueOnErrorReturnsInt) {
  expected<void, error> e{unexpected<error>(error::timeout)};
  auto result = e.transform([]() -> int { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(ExpectedVoid3Test, TransformConstLvalueOnValueReturnsInt) {
  const expected<void, error> e;
  auto result = e.transform([]() -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedVoid3Test, TransformConstLvalueOnErrorReturnsInt) {
  const expected<void, error> e{unexpected<error>(error::out_of_memory)};
  auto result = e.transform([]() -> int { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ================================================================
// Destructor coverage for additional template types (L149)
// ================================================================

TEST_F(ExpectedString3Test, DestructorValuePath) {
  { expected<std::string, error> e(std::string("hello")); }
}

TEST_F(ExpectedString3Test, DestructorErrorPath) {
  { expected<std::string, error> e{unexpected<error>(error::timeout)}; }
}

TEST_F(ExpectedInt3Test, DestructorValuePath) {
  { expected<int, error> e(42); }
}

TEST_F(ExpectedInt3Test, DestructorErrorPath) {
  { expected<int, error> e{unexpected<error>(error::timeout)}; }
}

} // namespace
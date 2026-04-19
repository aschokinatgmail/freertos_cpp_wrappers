#include <freertos_expected.hpp>

#include <gtest/gtest.h>
#include <string>

namespace {

using freertos::error;
using freertos::expected;
using freertos::unexpect;
using freertos::unexpected;

class ExpectedTMonadicTest : public ::testing::Test {};

// ---------- expected<T,E> copy/move constructors from error ---------

TEST_F(ExpectedTMonadicTest, CopyConstructorFromError) {
  expected<int, error> src(unexpected<error>(error::timeout));
  expected<int, error> dst(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::timeout);
}

TEST_F(ExpectedTMonadicTest, MoveConstructorFromError) {
  expected<int, error> src(unexpected<error>(error::queue_full));
  expected<int, error> dst(std::move(src));
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_full);
}

// ---------- expected<T,E> copy/move from unexpected (direct) ---------

TEST_F(ExpectedTMonadicTest, ConstructFromUnexpectedLvalue) {
  unexpected<error> u(error::would_block);
  expected<int, error> e(u);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::would_block);
}

TEST_F(ExpectedTMonadicTest, ConstructFromUnexpectedRvalue) {
  expected<int, error> e(unexpected<error>(error::out_of_memory));
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST_F(ExpectedTMonadicTest, ConstructFromUnexpectLvalue) {
  error err = error::invalid_parameter;
  expected<int, error> e(unexpect, err);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_parameter);
}

TEST_F(ExpectedTMonadicTest, ConstructFromUnexpectRvalue) {
  expected<int, error> e(unexpect, error::invalid_handle);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_handle);
}

// ---------- expected<T,E> copy assignment ---------

TEST_F(ExpectedTMonadicTest, CopyAssignmentSelfAssignment) {
  expected<int, error> e(42);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
  EXPECT_EQ(e.value(), 42);
}

TEST_F(ExpectedTMonadicTest, CopyAssignmentSelfAssignmentError) {
  expected<int, error> e(unexpected<error>(error::timeout));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedTMonadicTest, CopyAssignmentValueToError) {
  expected<int, error> src(99);
  expected<int, error> dst(unexpected<error>(error::timeout));
  dst = src;
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 99);
}

TEST_F(ExpectedTMonadicTest, CopyAssignmentErrorToValue) {
  expected<int, error> src(unexpected<error>(error::queue_empty));
  expected<int, error> dst(42);
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedTMonadicTest, CopyAssignmentErrorToError) {
  expected<int, error> src(unexpected<error>(error::would_block));
  expected<int, error> dst(unexpected<error>(error::timeout));
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- expected<T,E> move assignment ---------

TEST_F(ExpectedTMonadicTest, MoveAssignmentSelfAssignment) {
  expected<int, error> e(42);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
  EXPECT_EQ(e.value(), 42);
}

TEST_F(ExpectedTMonadicTest, MoveAssignmentSelfAssignmentError) {
  expected<int, error> e(unexpected<error>(error::queue_full));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::queue_full);
}

TEST_F(ExpectedTMonadicTest, MoveAssignmentValueToError) {
  expected<int, error> src(77);
  expected<int, error> dst(unexpected<error>(error::timeout));
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
  EXPECT_EQ(dst.value(), 77);
}

TEST_F(ExpectedTMonadicTest, MoveAssignmentErrorToValue) {
  expected<int, error> src(unexpected<error>(error::would_block));
  expected<int, error> dst(42);
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedTMonadicTest, MoveAssignmentErrorToError) {
  expected<int, error> src(unexpected<error>(error::invalid_handle));
  expected<int, error> dst(unexpected<error>(error::timeout));
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- expected<T,E> value() const&, value()&&, error() const&, error()&& ---------

TEST_F(ExpectedTMonadicTest, ValueConstRef) {
  const expected<int, error> e(42);
  EXPECT_EQ(e.value(), 42);
}

TEST_F(ExpectedTMonadicTest, ValueRvalueRef) {
  expected<int, error> e(42);
  int v = std::move(e).value();
  EXPECT_EQ(v, 42);
}

TEST_F(ExpectedTMonadicTest, ErrorConstRef) {
  const expected<int, error> e(unexpected<error>(error::timeout));
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedTMonadicTest, ErrorRvalueRef) {
  expected<int, error> e(unexpected<error>(error::queue_full));
  error err = std::move(e).error();
  EXPECT_EQ(err, error::queue_full);
}

// ---------- expected<T,E> operator* const& ---------

TEST_F(ExpectedTMonadicTest, OperatorDerefConstRef) {
  const expected<int, error> e(55);
  EXPECT_EQ(*e, 55);
}

// ---------- expected<T,E> and_then lvalue& on error ---------

TEST_F(ExpectedTMonadicTest, AndThenLvalueOnError) {
  expected<int, error> e(unexpected<error>(error::timeout));
  auto result = e.and_then([](int v) -> expected<int, error> {
    return expected<int, error>(v * 2);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- expected<T,E> and_then const& on value ---------

TEST_F(ExpectedTMonadicTest, AndThenConstLvalueOnValue) {
  const expected<int, error> e(10);
  auto result = e.and_then([](const int &v) -> expected<int, error> {
    return expected<int, error>(v * 3);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 30);
}

// ---------- expected<T,E> and_then const& on error ---------

TEST_F(ExpectedTMonadicTest, AndThenConstLvalueOnError) {
  const expected<int, error> e(unexpected<error>(error::would_block));
  auto result = e.and_then([](const int &v) -> expected<int, error> {
    return expected<int, error>(v * 3);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- expected<T,E> or_else lvalue& on value ---------

TEST_F(ExpectedTMonadicTest, OrElseLvalueOnValue) {
  expected<int, error> e(5);
  auto result = e.or_else([](error) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

// ---------- expected<T,E> or_else const& on value ---------

TEST_F(ExpectedTMonadicTest, OrElseConstLvalueOnValue) {
  const expected<int, error> e(5);
  auto result = e.or_else([](const error &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

// ---------- expected<T,E> or_else const& on error ---------

TEST_F(ExpectedTMonadicTest, OrElseConstLvalueOnError) {
  const expected<int, error> e(unexpected<error>(error::timeout));
  auto result = e.or_else([](const error &) -> expected<int, error> {
    return expected<int, error>(99);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 99);
}

// ---------- expected<T,E> transform lvalue& on error ---------

TEST_F(ExpectedTMonadicTest, TransformLvalueOnError) {
  expected<int, error> e(unexpected<error>(error::queue_empty));
  auto result = e.transform([](int v) { return v * 3; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::queue_empty);
}

// ---------- expected<T,E> transform const& on value ---------

TEST_F(ExpectedTMonadicTest, TransformConstLvalueOnValue) {
  const expected<int, error> e(7);
  auto result = e.transform([](const int &v) { return v + 1; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 8);
}

// ---------- expected<T,E> transform const& on error ---------

TEST_F(ExpectedTMonadicTest, TransformConstLvalueOnError) {
  const expected<int, error> e(unexpected<error>(error::out_of_memory));
  auto result = e.transform([](const int &v) { return v + 1; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ---------- expected<T,E> transform_error lvalue& on value ---------

TEST_F(ExpectedTMonadicTest, TransformErrorLvalueOnValue) {
  expected<int, error> e(5);
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

// ---------- expected<T,E> transform_error lvalue& on error ---------

TEST_F(ExpectedTMonadicTest, TransformErrorLvalueOnError) {
  expected<int, error> e(unexpected<error>(error::timeout));
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 42);
}

// ---------- expected<T,E> transform_error const& on value ---------

TEST_F(ExpectedTMonadicTest, TransformErrorConstLvalueOnValue) {
  const expected<int, error> e(5);
  auto result = e.transform_error([](const error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

// ---------- expected<T,E> transform_error const& on error ---------

TEST_F(ExpectedTMonadicTest, TransformErrorConstLvalueOnError) {
  const expected<int, error> e(unexpected<error>(error::invalid_handle));
  auto result = e.transform_error([](const error &) -> int { return 99; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 99);
}

// ---------- unexpected accessors ---------

TEST_F(ExpectedTMonadicTest, UnexpectedValueLvalue) {
  unexpected<error> u(error::timeout);
  EXPECT_EQ(u.value(), error::timeout);
}

TEST_F(ExpectedTMonadicTest, UnexpectedValueConstRvalue) {
  const unexpected<error> u(error::queue_full);
  error e = std::move(u).value();
  EXPECT_EQ(e, error::queue_full);
}

TEST_F(ExpectedTMonadicTest, UnexpectedValueRvalue) {
  unexpected<error> u(error::would_block);
  error e = std::move(u).value();
  EXPECT_EQ(e, error::would_block);
}

// ================================================================
// expected<void, E> comprehensive monadic tests
// ================================================================

class ExpectedVoidMonadicTest : public ::testing::Test {};

// ---------- expected<void,E> copy/move from error ---------

TEST_F(ExpectedVoidMonadicTest, CopyConstructorFromError) {
  expected<void, error> src(unexpected<error>(error::timeout));
  expected<void, error> dst(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::timeout);
}

TEST_F(ExpectedVoidMonadicTest, MoveConstructorFromError) {
  expected<void, error> src(unexpected<error>(error::queue_empty));
  expected<void, error> dst(std::move(src));
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

// ---------- expected<void,E> copy/move from unexpected ---------

TEST_F(ExpectedVoidMonadicTest, ConstructFromUnexpectedLvalue) {
  unexpected<error> u(error::would_block);
  expected<void, error> e(u);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::would_block);
}

TEST_F(ExpectedVoidMonadicTest, ConstructFromUnexpectedRvalue) {
  expected<void, error> e(unexpected<error>(error::out_of_memory));
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::out_of_memory);
}

TEST_F(ExpectedVoidMonadicTest, ConstructFromUnexpectLvalue) {
  error err = error::invalid_parameter;
  expected<void, error> e(unexpect, err);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_parameter);
}

TEST_F(ExpectedVoidMonadicTest, ConstructFromUnexpectRvalue) {
  expected<void, error> e(unexpect, error::invalid_handle);
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::invalid_handle);
}

// ---------- expected<void,E> copy assignment ---------

TEST_F(ExpectedVoidMonadicTest, CopyAssignmentSelfAssignmentValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoidMonadicTest, CopyAssignmentSelfAssignmentError) {
  expected<void, error> e(unexpected<error>(error::timeout));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
  e = e;
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedVoidMonadicTest, CopyAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst(unexpected<error>(error::timeout));
  dst = src;
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidMonadicTest, CopyAssignmentErrorToValue) {
  expected<void, error> src(unexpected<error>(error::queue_empty));
  expected<void, error> dst;
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::queue_empty);
}

TEST_F(ExpectedVoidMonadicTest, CopyAssignmentErrorToError) {
  expected<void, error> src(unexpected<error>(error::would_block));
  expected<void, error> dst(unexpected<error>(error::timeout));
  dst = src;
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

// ---------- expected<void,E> move assignment ---------

TEST_F(ExpectedVoidMonadicTest, MoveAssignmentSelfAssignmentValue) {
  expected<void, error> e;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_TRUE(e.has_value());
}

TEST_F(ExpectedVoidMonadicTest, MoveAssignmentSelfAssignmentError) {
  expected<void, error> e(unexpected<error>(error::queue_full));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  e = std::move(e);
#pragma GCC diagnostic pop
  EXPECT_FALSE(e.has_value());
  EXPECT_EQ(e.error(), error::queue_full);
}

TEST_F(ExpectedVoidMonadicTest, MoveAssignmentValueToError) {
  expected<void, error> src;
  expected<void, error> dst(unexpected<error>(error::timeout));
  dst = std::move(src);
  EXPECT_TRUE(dst.has_value());
}

TEST_F(ExpectedVoidMonadicTest, MoveAssignmentErrorToValue) {
  expected<void, error> src(unexpected<error>(error::would_block));
  expected<void, error> dst;
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::would_block);
}

TEST_F(ExpectedVoidMonadicTest, MoveAssignmentErrorToError) {
  expected<void, error> src(unexpected<error>(error::invalid_handle));
  expected<void, error> dst(unexpected<error>(error::timeout));
  dst = std::move(src);
  EXPECT_FALSE(dst.has_value());
  EXPECT_EQ(dst.error(), error::invalid_handle);
}

// ---------- expected<void,E> error() const&, error()&& ---------

TEST_F(ExpectedVoidMonadicTest, ErrorConstRef) {
  const expected<void, error> e(unexpected<error>(error::timeout));
  EXPECT_EQ(e.error(), error::timeout);
}

TEST_F(ExpectedVoidMonadicTest, ErrorRvalueRef) {
  expected<void, error> e(unexpected<error>(error::queue_full));
  error err = std::move(e).error();
  EXPECT_EQ(err, error::queue_full);
}

// ---------- expected<void,E> and_then lvalue& on error ---------

TEST_F(ExpectedVoidMonadicTest, AndThenLvalueOnError) {
  expected<void, error> e(unexpected<error>(error::timeout));
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- expected<void,E> and_then const& on value ---------

TEST_F(ExpectedVoidMonadicTest, AndThenConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

// ---------- expected<void,E> and_then const& on error ---------

TEST_F(ExpectedVoidMonadicTest, AndThenConstLvalueOnError) {
  const expected<void, error> e(unexpected<error>(error::would_block));
  auto result = e.and_then([]() -> expected<int, error> {
    return expected<int, error>(42);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ---------- expected<void,E> or_else lvalue& on value ---------

TEST_F(ExpectedVoidMonadicTest, OrElseLvalueOnValue) {
  expected<void, error> e;
  auto result = e.or_else([](error) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> or_else lvalue& on error ---------

TEST_F(ExpectedVoidMonadicTest, OrElseLvalueOnError) {
  expected<void, error> e(unexpected<error>(error::timeout));
  auto result = e.or_else([](error) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> or_else const& on value ---------

TEST_F(ExpectedVoidMonadicTest, OrElseConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> or_else const& on error ---------

TEST_F(ExpectedVoidMonadicTest, OrElseConstLvalueOnError) {
  const expected<void, error> e(unexpected<error>(error::queue_empty));
  auto result = e.or_else([](const error &) -> expected<void, error> {
    return expected<void, error>();
  });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> transform lvalue& on error ---------

TEST_F(ExpectedVoidMonadicTest, TransformLvalueOnError) {
  expected<void, error> e(unexpected<error>(error::timeout));
  auto result = e.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ---------- expected<void,E> transform const& on value ---------

TEST_F(ExpectedVoidMonadicTest, TransformConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

// ---------- expected<void,E> transform const& on error ---------

TEST_F(ExpectedVoidMonadicTest, TransformConstLvalueOnError) {
  const expected<void, error> e(unexpected<error>(error::out_of_memory));
  auto result = e.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::out_of_memory);
}

// ---------- expected<void,E> transform_error lvalue& on value ---------

TEST_F(ExpectedVoidMonadicTest, TransformErrorLvalueOnValue) {
  expected<void, error> e;
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> transform_error lvalue& on error ---------

TEST_F(ExpectedVoidMonadicTest, TransformErrorLvalueOnError) {
  expected<void, error> e(unexpected<error>(error::timeout));
  auto result = e.transform_error([](error) -> int { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 42);
}

// ---------- expected<void,E> transform_error const& on value ---------

TEST_F(ExpectedVoidMonadicTest, TransformErrorConstLvalueOnValue) {
  const expected<void, error> e;
  auto result = e.transform_error([](const error &) -> int { return 42; });
  EXPECT_TRUE(result.has_value());
}

// ---------- expected<void,E> transform_error const& on error ---------

TEST_F(ExpectedVoidMonadicTest, TransformErrorConstLvalueOnError) {
  const expected<void, error> e(unexpected<error>(error::invalid_handle));
  auto result = e.transform_error([](const error &) -> int { return 99; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), 99);
}

// ---------- expected<void,E> destructor path: value then error ---------

TEST_F(ExpectedVoidMonadicTest, DestructorValuePath) {
  { expected<void, error> e; }
}

TEST_F(ExpectedVoidMonadicTest, DestructorErrorPath) {
  { expected<void, error> e(unexpected<error>(error::timeout)); }
}

// ---------- expected<T,E> destructor path: value then error ---------

TEST_F(ExpectedTMonadicTest, DestructorValuePath) {
  { expected<int, error> e(42); }
}

TEST_F(ExpectedTMonadicTest, DestructorErrorPath) {
  { expected<int, error> e(unexpected<error>(error::timeout)); }
}

// ---------- String-typed T for branch coverage ---------

TEST_F(ExpectedTMonadicTest, StringExpectedValue) {
  expected<std::string, error> e(std::string("hello"));
  EXPECT_TRUE(e.has_value());
  EXPECT_EQ(e.value(), "hello");
}

TEST_F(ExpectedTMonadicTest, StringExpectedCopyConstruct) {
  expected<std::string, error> e1(std::string("world"));
  expected<std::string, error> e2(e1);
  EXPECT_EQ(e2.value(), "world");
}

TEST_F(ExpectedTMonadicTest, StringExpectedMoveConstruct) {
  expected<std::string, error> e1(std::string("move"));
  expected<std::string, error> e2(std::move(e1));
  EXPECT_EQ(e2.value(), "move");
}

TEST_F(ExpectedTMonadicTest, StringExpectedAndThenValue) {
  expected<std::string, error> e(std::string("abc"));
  auto result = e.and_then([](std::string &s) -> expected<std::string, error> {
    return expected<std::string, error>(s + "def");
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abcdef");
}

TEST_F(ExpectedTMonadicTest, StringExpectedOrElseValue) {
  expected<std::string, error> e(std::string("keep"));
  auto result = e.or_else([](error) -> expected<std::string, error> {
    return expected<std::string, error>(std::string("replaced"));
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "keep");
}

TEST_F(ExpectedTMonadicTest, StringExpectedTransformValue) {
  expected<std::string, error> e(std::string("abc"));
  auto result = e.transform([](std::string &s) -> size_t { return s.size(); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3u);
}

// ---------- operator-> const ---------

TEST_F(ExpectedTMonadicTest, OperatorArrowConst) {
  const expected<std::string, error> e(std::string("test"));
  EXPECT_EQ(e->size(), 4u);
}

} // namespace
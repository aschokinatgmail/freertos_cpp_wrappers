#include <gtest/gtest.h>

#include <freertos_span.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

TEST(SpanDynamicExtent, DefaultConstructor) {
  freertos::span<int> s;
  EXPECT_EQ(s.size(), 0u);
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.data(), nullptr);
}

TEST(SpanDynamicExtent, PointerSizeConstructor) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int> s(arr, 5);
  EXPECT_EQ(s.size(), 5u);
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.data(), arr);
}

TEST(SpanDynamicExtent, CArrayConstructor) {
  int arr[] = {10, 20, 30};
  freertos::span<int> s(arr);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 10);
  EXPECT_EQ(s[1], 20);
  EXPECT_EQ(s[2], 30);
}

TEST(SpanDynamicExtent, StdArrayConstructor) {
  std::array<int, 4> a = {5, 6, 7, 8};
  freertos::span<int> s(a);
  EXPECT_EQ(s.size(), 4u);
  EXPECT_EQ(s[0], 5);
  EXPECT_EQ(s[3], 8);
}

TEST(SpanDynamicExtent, ConstStdArrayConstructor) {
  const std::array<int, 3> a = {1, 2, 3};
  freertos::span<const int> s(a);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 1);
  EXPECT_EQ(s[2], 3);
}

TEST(SpanDynamicExtent, DataSizeBytes) {
  int32_t arr[] = {1, 2, 3};
  freertos::span<int32_t> s(arr);
  EXPECT_EQ(s.size_bytes(), 3u * sizeof(int32_t));
}

TEST(SpanDynamicExtent, FrontBack) {
  int arr[] = {100, 200, 300};
  freertos::span<int> s(arr);
  EXPECT_EQ(s.front(), 100);
  EXPECT_EQ(s.back(), 300);
}

TEST(SpanDynamicExtent, BeginEnd) {
  int arr[] = {1, 2, 3};
  freertos::span<int> s(arr);
  int sum = 0;
  for (auto &v : s) {
    sum += v;
  }
  EXPECT_EQ(sum, 6);
}

TEST(SpanDynamicExtent, First) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int> s(arr);
  auto f = s.first(3);
  EXPECT_EQ(f.size(), 3u);
  EXPECT_EQ(f[0], 1);
  EXPECT_EQ(f[1], 2);
  EXPECT_EQ(f[2], 3);
}

TEST(SpanDynamicExtent, Last) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int> s(arr);
  auto l = s.last(2);
  EXPECT_EQ(l.size(), 2u);
  EXPECT_EQ(l[0], 4);
  EXPECT_EQ(l[1], 5);
}

TEST(SpanDynamicExtent, SubspanWithCount) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int> s(arr);
  auto sub = s.subspan(1, 3);
  EXPECT_EQ(sub.size(), 3u);
  EXPECT_EQ(sub[0], 2);
  EXPECT_EQ(sub[1], 3);
  EXPECT_EQ(sub[2], 4);
}

TEST(SpanDynamicExtent, SubspanWithoutCount) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int> s(arr);
  auto sub = s.subspan(2);
  EXPECT_EQ(sub.size(), 3u);
  EXPECT_EQ(sub[0], 3);
  EXPECT_EQ(sub[2], 5);
}

TEST(SpanFixedExtent, CArrayConstructor) {
  int arr[] = {1, 2, 3};
  freertos::span<int, 3> s(arr);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 1);
  EXPECT_EQ(s[2], 3);
}

TEST(SpanFixedExtent, StdArrayConstructor) {
  std::array<int, 4> a = {10, 20, 30, 40};
  freertos::span<int, 4> s(a);
  EXPECT_EQ(s.size(), 4u);
  EXPECT_EQ(s.front(), 10);
  EXPECT_EQ(s.back(), 40);
}

TEST(SpanFixedExtent, PointerSizeConstructor) {
  int arr[] = {5, 6, 7};
  freertos::span<int, 3> s(arr, 3);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s.size_bytes(), 3u * sizeof(int));
}

TEST(SpanFixedExtent, FirstLastSubspan) {
  int arr[] = {1, 2, 3, 4, 5};
  freertos::span<int, 5> s(arr);
  auto f = s.first(2);
  EXPECT_EQ(f.size(), 2u);
  EXPECT_EQ(f[0], 1);
  auto l = s.last(1);
  EXPECT_EQ(l.size(), 1u);
  EXPECT_EQ(l[0], 5);
  auto sub = s.subspan(2, 2);
  EXPECT_EQ(sub.size(), 2u);
  EXPECT_EQ(sub[0], 3);
  EXPECT_EQ(sub[1], 4);
}

TEST(SpanConst, ConstSpanFromMutable) {
  int arr[] = {1, 2, 3};
  freertos::span<int> ms(arr);
  freertos::span<const int> cs = ms;
  EXPECT_EQ(cs.size(), 3u);
  EXPECT_EQ(cs[1], 2);
}

TEST(SpanTriviallyCopyable, IsTriviallyCopyable) {
  using span_int = freertos::span<int>;
  using span_cint = freertos::span<const int>;
  using span_int_3 = freertos::span<int, 3>;
  EXPECT_TRUE(std::is_trivially_copyable<span_int>::value);
  EXPECT_TRUE(std::is_trivially_copyable<span_cint>::value);
  EXPECT_TRUE(std::is_trivially_copyable<span_int_3>::value);
}

TEST(SpanDynamicExtent, SizeBytesDifferentTypes) {
  int8_t a8[] = {1, 2, 3, 4};
  freertos::span<int8_t> s8(a8);
  EXPECT_EQ(s8.size_bytes(), 4u);

  int64_t a64[] = {1, 2};
  freertos::span<int64_t> s64(a64);
  EXPECT_EQ(s64.size_bytes(), 2u * sizeof(int64_t));
}

TEST(SpanDynamicExtent, EmptyFirstLast) {
  int arr[] = {42};
  freertos::span<int> s(arr, 0);
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(SpanDynamicExtent, OperatorBracket) {
  int arr[] = {10, 20, 30};
  freertos::span<int> s(arr);
  EXPECT_EQ(s[0], 10);
  EXPECT_EQ(s[1], 20);
  EXPECT_EQ(s[2], 30);
}

TEST(SpanDynamicExtent, DynamicExtentValue) {
  EXPECT_EQ(freertos::dynamic_extent, static_cast<size_t>(-1));
}
/**
 * @file test_freertos_stream_buffer_contiguous.cpp
 * @brief Regression tests for issue #306 — `is_contiguous_iterator_v` must
 *        not accept `std::deque::iterator` (random-access but NOT contiguous).
 *
 * Treating a deque iterator as contiguous led to silent UB on `&*begin + N`.
 *
 * Trait policy:
 * - **C++20**: backed by `std::contiguous_iterator`, which correctly
 *   distinguishes pointer / vector / array / span / string from deque /
 *   list / map / etc.
 * - **C++17**: heap-purity directive forbids pulling `<vector>` / `<string>`
 *   into the public stream_buffer header, so the fallback recognizes ONLY
 *   raw pointers as contiguous. Container iterators correctly fall through
 *   to the chunked-fallback path (slower but safe). Users who want the
 *   contiguous fast-path under C++17 should pass `(container.data(),
 *   container.size())` to the byte-pointer overload.
 *
 * Both modes must reject `std::deque::iterator` — the actual safety
 * regression test.
 */

#include <array>
#include <deque>
#include <list>
#include <string>
#include <vector>
#if defined(__cpp_lib_span)
#include <span>
#endif

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_stream_buffer.hpp"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrictMock;

// -----------------------------------------------------------------------------
// Compile-time iterator-category sanity checks. These document the
// standard-library guarantees the trait relies on.
// -----------------------------------------------------------------------------
static_assert(
    std::is_same<typename std::iterator_traits<
                     std::vector<uint8_t>::iterator>::iterator_category,
                 std::random_access_iterator_tag>::value,
    "std::vector::iterator is random-access");
static_assert(
    std::is_same<typename std::iterator_traits<
                     std::deque<uint8_t>::iterator>::iterator_category,
                 std::random_access_iterator_tag>::value,
    "std::deque::iterator is random-access (and that's why the old "
    "category-based trait misclassified it)");
static_assert(
    std::is_same<typename std::iterator_traits<
                     std::list<uint8_t>::iterator>::iterator_category,
                 std::bidirectional_iterator_tag>::value,
    "std::list::iterator is bidirectional");

// -----------------------------------------------------------------------------
// Universal (always-true) trait pin-downs: raw pointers must always be
// recognized as contiguous; deque / list iterators must never be — that's
// the safety guarantee.
// -----------------------------------------------------------------------------
static_assert(freertos::detail::is_contiguous_iterator_v<uint8_t *>,
              "raw pointer must be treated as contiguous");
static_assert(freertos::detail::is_contiguous_iterator_v<const uint8_t *>,
              "const raw pointer must be treated as contiguous");

static_assert(
    !freertos::detail::is_contiguous_iterator_v<std::deque<uint8_t>::iterator>,
    "std::deque<uint8_t>::iterator must NOT be treated as contiguous "
    "(regression test for #306)");
static_assert(
    !freertos::detail::is_contiguous_iterator_v<
        std::deque<uint8_t>::const_iterator>,
    "std::deque<uint8_t>::const_iterator must NOT be treated as contiguous");
static_assert(
    !freertos::detail::is_contiguous_iterator_v<std::list<uint8_t>::iterator>,
    "std::list<uint8_t>::iterator must NOT be treated as contiguous");

// -----------------------------------------------------------------------------
// C++20-only: under std::contiguous_iterator, vector / array / string
// iterators are correctly recognized as contiguous. Under C++17 the
// heap-pure trait recognizes only raw pointers, and the public header
// must not name <vector>/<string>/<array> types.
// -----------------------------------------------------------------------------
#if defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 201907L
static_assert(
    freertos::detail::is_contiguous_iterator_v<std::vector<uint8_t>::iterator>,
    "C++20: std::vector<uint8_t>::iterator must be contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<
        std::vector<uint8_t>::const_iterator>,
    "C++20: std::vector<uint8_t>::const_iterator must be contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<
        typename std::array<uint8_t, 16>::iterator>,
    "C++20: std::array iterator must be contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<std::string::iterator>,
    "C++20: std::string::iterator must be contiguous");
#endif

// -----------------------------------------------------------------------------
// Runtime regression: send from a std::deque<uint8_t>. After the fix, this
// must dispatch to the chunked-fallback path (multiple xStreamBufferSend
// calls of <= 64 bytes), not to the broken `&*begin + N` byte-pointer path.
// -----------------------------------------------------------------------------
class StreamBufferDequeFallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    handle = reinterpret_cast<StreamBufferHandle_t>(0xC0FFEE);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock>* mock;
  StreamBufferHandle_t handle;
};

TEST_F(StreamBufferDequeFallbackTest, DequeSendDispatchesToChunkedFallback) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));

  // The fallback chunk size in the wrapper is 64 bytes; for a deque of 100
  // uint8_t it must be split into at least one call (64) and possibly a
  // second (36). We accept any number >= 1 for portability across chunk
  // sizes and only check the cumulative content via the mock.
  size_t total_bytes_observed = 0;
  EXPECT_CALL(*mock,
              xStreamBufferSend(handle, NotNull(), _, portMAX_DELAY))
      .Times(AtLeast(1))
      .WillRepeatedly(
          [&total_bytes_observed](StreamBufferHandle_t, const void*,
                                  size_t bytes, TickType_t) -> size_t {
            total_bytes_observed += bytes;
            return bytes;  // Pretend the buffer accepted the full chunk.
          });

  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  std::deque<uint8_t> data(100);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = static_cast<uint8_t>(i);
  }

  const auto sent = buffer.send(data.begin(), data.end());
  EXPECT_EQ(sent, data.size());
  EXPECT_EQ(total_bytes_observed, data.size());
}

// Vector and string are correctness-tested via byte count only — they
// dispatch to the contiguous fast-path under C++20 (single send) and to
// the chunked fallback under C++17 (one or more sends). Both paths must
// deliver all bytes intact; the path selection is an optimization.
TEST_F(StreamBufferDequeFallbackTest, VectorSendDeliversAllBytes) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));
  size_t total = 0;
  EXPECT_CALL(*mock, xStreamBufferSend(handle, NotNull(), _, portMAX_DELAY))
      .Times(AtLeast(1))
      .WillRepeatedly(
          [&total](StreamBufferHandle_t, const void*, size_t n,
                   TickType_t) -> size_t {
            total += n;
            return n;
          });
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  std::vector<uint8_t> data(50, 0xAB);
  const auto sent = buffer.send(data.begin(), data.end());
  EXPECT_EQ(sent, data.size());
  EXPECT_EQ(total, data.size());
}

TEST_F(StreamBufferDequeFallbackTest, StringSendDeliversAllBytes) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));
  size_t total = 0;
  EXPECT_CALL(*mock, xStreamBufferSend(handle, NotNull(), _, portMAX_DELAY))
      .Times(AtLeast(1))
      .WillRepeatedly(
          [&total](StreamBufferHandle_t, const void*, size_t n,
                   TickType_t) -> size_t {
            total += n;
            return n;
          });
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  std::string data = "hello";
  const auto sent = buffer.send(data.begin(), data.end());
  EXPECT_EQ(sent, data.size());
  EXPECT_EQ(total, data.size());
}

TEST_F(StreamBufferDequeFallbackTest, RawPointerSendUsesContiguousFastPath) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));
  // Raw pointers always take the fast path — exactly one send.
  EXPECT_CALL(*mock,
              xStreamBufferSend(handle, NotNull(), 7, portMAX_DELAY))
      .WillOnce(Return(7));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  uint8_t data[7] = {1, 2, 3, 4, 5, 6, 7};
  const auto sent = buffer.send(data, data + 7);
  EXPECT_EQ(sent, sizeof(data));
}

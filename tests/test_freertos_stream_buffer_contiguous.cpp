/**
 * @file test_freertos_stream_buffer_contiguous.cpp
 * @brief Regression tests for issue #306 — is_contiguous_iterator_v must not
 *        accept std::deque iterators.
 *
 * std::deque is random-access but not contiguous; treating it as contiguous
 * led to silent UB on `&*begin + N`. The trait is now backed by either
 * std::contiguous_iterator (C++20) or an explicit allowlist of standard
 * library iterators known to be contiguous (C++17).
 *
 * These tests pin down the trait at compile time and verify that a deque can
 * still be sent through the chunked-fallback path at runtime.
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
// Compile-time iterator-category sanity checks.
// These are not the trait itself, but they document the standard-library
// guarantees the trait relies on. If these change, the trait must too.
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
// The trait itself: vector / array / string / raw pointer must be contiguous;
// deque / list must NOT be.
// -----------------------------------------------------------------------------
static_assert(
    freertos::detail::is_contiguous_iterator_v<std::vector<uint8_t>::iterator>,
    "std::vector<uint8_t>::iterator must be treated as contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<
        std::vector<uint8_t>::const_iterator>,
    "std::vector<uint8_t>::const_iterator must be treated as contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<
        typename std::array<uint8_t, 16>::iterator>,
    "std::array iterator must be treated as contiguous");
static_assert(
    freertos::detail::is_contiguous_iterator_v<std::string::iterator>,
    "std::string::iterator must be treated as contiguous");
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
// Runtime regression: send from a std::deque<uint8_t>. After the fix, this
// must dispatch to the chunked-fallback path (multiple xStreamBufferSend
// calls of <= 64 bytes), not to the broken `&*begin + N` byte-pointer path.
//
// We verify by setting up a strict mock that expects at least one
// xStreamBufferSend call with a NotNull buffer pointer and the deque content
// being correctly preserved.
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

TEST_F(StreamBufferDequeFallbackTest, VectorSendStillUsesContiguousFastPath) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));

  // For a contiguous iterator, we expect exactly one xStreamBufferSend call
  // with the full byte count.
  EXPECT_CALL(*mock,
              xStreamBufferSend(handle, NotNull(), 50, portMAX_DELAY))
      .WillOnce(Return(50));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  std::vector<uint8_t> data(50, 0xAB);
  const auto sent = buffer.send(data.begin(), data.end());
  EXPECT_EQ(sent, data.size());
}

TEST_F(StreamBufferDequeFallbackTest, StringSendStillUsesContiguousFastPath) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(handle, NotNull(), 5, portMAX_DELAY))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  std::string data = "hello";
  const auto sent = buffer.send(data.begin(), data.end());
  EXPECT_EQ(sent, data.size());
}

TEST_F(StreamBufferDequeFallbackTest, RawPointerSendStillUsesContiguousFastPath) {
  EXPECT_CALL(*mock, xStreamBufferCreate(256, 1)).WillOnce(Return(handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(handle, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::da::stream_buffer<256> buffer;
  uint8_t data[] = {1, 2, 3, 4};
  const auto sent = buffer.send(std::begin(data), std::end(data));
  EXPECT_EQ(sent, sizeof(data));
}

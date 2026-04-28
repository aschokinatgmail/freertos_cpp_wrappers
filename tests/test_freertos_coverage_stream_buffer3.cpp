#include <array>
#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/freertos_mocks.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::WithArg;

class SAStreamBuffer256Test : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
    sb_handle = reinterpret_cast<StreamBufferHandle_t>(0x1000);
    sb_handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2000);
  }

  void TearDown() override {
    FreeRTOSMockInstance::resetInstance();
    mock.reset();
  }

  std::shared_ptr<StrictMock<FreeRTOSMock>> mock;
  StreamBufferHandle_t sb_handle;
  StreamBufferHandle_t sb_handle2;
};

class ExtAllocStreamBuffer64Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock_ptr;

  static void *test_allocate(size_t size) { return malloc(size); }
  static void test_deallocate(void *ptr) { free(ptr); }

  void SetUp() override {
    mock_ptr = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock_ptr;
  }

  void TearDown() override {
    delete mock_ptr;
    g_freertos_mock = nullptr;
  }
};

using TestMemoryRegion = freertos::external_memory_region;

// =============================================================================
// 1. freertos::static_stream_buffer_allocator<256>::swap
// =============================================================================

TEST_F(SAStreamBuffer256Test, StaticAllocatorSwap) {
  freertos::static_stream_buffer_allocator<256> alloc1;
  freertos::static_stream_buffer_allocator<256> alloc2;
  alloc1.swap(alloc2);
}

// =============================================================================
// 2. SFINAE constructor with StreamArgAllocator (external_stream_buffer_allocator)
//    freertos::stream_buffer<64, StreamArgAllocator>::stream_buffer(trigger_level, allocator_ref)
// =============================================================================

TEST_F(ExtAllocStreamBuffer64Test, SFINAECtorWithExternalAllocator) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x5000);
  TestMemoryRegion region{test_allocate, test_deallocate};
  EXPECT_CALL(*mock_ptr, xStreamBufferCreateStatic(64, 4, NotNull(), NotNull()))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock_ptr, vStreamBufferDelete(h));

  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc(region);
  freertos::stream_buffer<64, freertos::external_stream_buffer_allocator<TestMemoryRegion, 64>> sb(4, std::move(alloc));
}

// =============================================================================
// 3. Destructor (non-null handle path) — covered implicitly by construction.
//    Also cover the null-handle destructor path via move-from object.
// =============================================================================

TEST_F(SAStreamBuffer256Test, DestructorAfterMove) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> sb1;
  freertos::sa::stream_buffer<256> sb2(std::move(sb1));
}

// =============================================================================
// 4. Move assignment operator= for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, MoveAssignmentDifferent) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::sa::stream_buffer<256> sb1;
  freertos::sa::stream_buffer<256> sb2;
  sb1 = std::move(sb2);
  EXPECT_EQ(sb1.handle(), sb_handle2);
}

TEST_F(SAStreamBuffer256Test, MoveAssignmentSelf) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> sb1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sb1 = std::move(sb1);
#pragma GCC diagnostic pop
  EXPECT_EQ(sb1.handle(), sb_handle);
}

// =============================================================================
// 5. swap method for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, SwapMethod) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::sa::stream_buffer<256> sb1;
  freertos::sa::stream_buffer<256> sb2;
  sb1.swap(sb2);
  EXPECT_EQ(sb1.handle(), sb_handle2);
  EXPECT_EQ(sb2.handle(), sb_handle);
}

// =============================================================================
// 6. send_isr for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, SendIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBuffer256Test, SendIsr_WokenFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// =============================================================================
// 7. receive_isr for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, ReceiveIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBuffer256Test, ReceiveIsr_WokenFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// =============================================================================
// 8. reset for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, Reset) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  EXPECT_EQ(buf.reset(), pdPASS);
}

// =============================================================================
// 9. set_trigger_level for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, SetTriggerLevel) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 8))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  EXPECT_EQ(buf.set_trigger_level(8), pdPASS);
}

// =============================================================================
// 10. send_ex_isr for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, SendExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBuffer256Test, SendExIsr_FailureWouldBlock) {
  // ISR send returned 0 but buffer is not full — surface would_block.
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(sb_handle)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(SAStreamBuffer256Test, SendExIsr_FailureBufferFull) {
  // ISR send returned 0 because the buffer is full — surface buffer_full.
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsFull(sb_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_full);
}

// =============================================================================
// 11. receive_ex_isr for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, ReceiveExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBuffer256Test, ReceiveExIsr_FailureWouldBlock) {
  // ISR receive returned 0 but buffer is not empty — surface would_block.
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(sb_handle)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(SAStreamBuffer256Test, ReceiveExIsr_FailureBufferEmpty) {
  // ISR receive returned 0 because the buffer is empty — surface buffer_empty.
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(sb_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::buffer_empty);
}

// =============================================================================
// 12. reset_ex for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, ResetEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SAStreamBuffer256Test, ResetEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// =============================================================================
// 13. set_trigger_level_ex for sa::stream_buffer<256>
// =============================================================================

TEST_F(SAStreamBuffer256Test, SetTriggerLevelEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 8))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  auto r = buf.set_trigger_level_ex(8);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SAStreamBuffer256Test, SetTriggerLevelEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 300))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<256> buf;
  auto r = buf.set_trigger_level_ex(300);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_parameter);
}
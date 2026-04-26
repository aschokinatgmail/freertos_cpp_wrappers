#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_queue.hpp"
#include "freertos_isr_result.hpp"
#include "mocks/freertos_mocks.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

using Q = freertos::sa::queue<10, int>;
using Alloc = freertos::static_queue_allocator<10, int>;

class SaQueue2Test : public ::testing::Test {
protected:
  std::shared_ptr<StrictMock<FreeRTOSMock>> mock_ptr;
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x9001);

  void SetUp() override {
    mock_ptr = FreeRTOSMockInstance::getInstance();
    mock = mock_ptr.get();
  }
  void TearDown() override {
    FreeRTOSMockInstance::resetInstance();
    mock = nullptr;
  }
};

// ================================================================
// SFINAE CONSTRUCTOR WITH ALLOCATOR ARGS (lines 213-225)
// ================================================================

TEST_F(SaQueue2Test, SfinaeConstructorNullName) {
  Alloc alloc;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q(nullptr, std::move(alloc));
}

TEST_F(SaQueue2Test, SfinaeConstructorRegisteredName) {
  Alloc alloc;
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vQueueAddToRegistry(h, _));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return("sfinae_q"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(h));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q("sfinae_q", std::move(alloc));
}

// ================================================================
// DESTRUCTOR WITH REGISTERED NAME (lines 235-236)
// ================================================================

TEST_F(SaQueue2Test, DestructorUnregistersNamedQueue) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vQueueAddToRegistry(h, _));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return("named_q"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(h));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q("named_q");
}

TEST_F(SaQueue2Test, DestructorSkipsUnregisterUnnamedQueue) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
}

// ================================================================
// MOVE CONSTRUCTOR (lines 227-229)
// ================================================================

TEST_F(SaQueue2Test, MoveConstructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q1;
  Q q2(std::move(q1));
}

// ================================================================
// MOVE ASSIGNMENT (lines 242-247) + SWAP (lines 249-253, 255, 69,71-73)
// ================================================================

TEST_F(SaQueue2Test, MoveAssignmentDifferentObjects) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x9011);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0x9012);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, pcQueueGetName(h1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  EXPECT_CALL(*mock, pcQueueGetName(h2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h2));
  Q q1;
  Q q2;
  q1 = std::move(q2);
}

TEST_F(SaQueue2Test, MemberSwap) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x9011);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0x9012);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, pcQueueGetName(h1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  EXPECT_CALL(*mock, pcQueueGetName(h2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h2));
  Q q1;
  Q q2;
  q1.swap(q2);
}

TEST_F(SaQueue2Test, FriendSwap) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x9011);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0x9012);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, pcQueueGetName(h1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  EXPECT_CALL(*mock, pcQueueGetName(h2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h2));
  Q q1;
  Q q2;
  using std::swap;
  swap(q1, q2);
}

// ================================================================
// SEND_ISR / RECEIVE_ISR (lines 296-301, 460-465, 475-485)
// ================================================================

TEST_F(SaQueue2Test, SendIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaQueue2Test, SendIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_isr(42);
  EXPECT_EQ(r.result, errQUEUE_FULL);
}

TEST_F(SaQueue2Test, ReceiveIsrRefSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.receive_isr(val);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaQueue2Test, ReceiveIsrRefFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.receive_isr(val);
  EXPECT_EQ(r.result, pdFALSE);
}

TEST_F(SaQueue2Test, ReceiveIsrOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueue2Test, ReceiveIsrOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_isr();
  EXPECT_FALSE(r.result.has_value());
}

// ================================================================
// SEND_EX FAILURE PATHS (lines 666, 668)
// ================================================================

TEST_F(SaQueue2Test, SendExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(SaQueue2Test, SendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_ex(42, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, SendExChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_ex(42, std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// ================================================================
// SEND_EX_ISR SUCCESS AND FAILURE (lines 684-685)
// ================================================================

TEST_F(SaQueue2Test, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueue2Test, SendExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendFromISR(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_ex_isr(42);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::queue_full);
}

// ================================================================
// SEND_BACK_EX FAILURE PATHS (lines 692)
// ================================================================

TEST_F(SaQueue2Test, SendBackExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(SaQueue2Test, SendBackExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_ex(42, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, SendBackExChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_ex(42, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// ================================================================
// SEND_BACK_EX_ISR (lines 712-713)
// ================================================================

TEST_F(SaQueue2Test, SendBackExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueue2Test, SendBackExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_ex_isr(42);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::queue_full);
}

// ================================================================
// SEND_FRONT_EX FAILURE PATHS (lines 720)
// ================================================================

TEST_F(SaQueue2Test, SendFrontExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_ex(42, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

TEST_F(SaQueue2Test, SendFrontExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, 100)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_ex(42, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, SendFrontExChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_ex(42, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// ================================================================
// SEND_FRONT_EX_ISR (lines 740-741)
// ================================================================

TEST_F(SaQueue2Test, SendFrontExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_ex_isr(42);
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueue2Test, SendFrontExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(h, _, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_ex_isr(42);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::queue_full);
}

// ================================================================
// RECEIVE_EX FAILURE PATHS (lines 748, 750, 764, 766)
// ================================================================

TEST_F(SaQueue2Test, ReceiveExRefWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.receive_ex(val, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_empty);
}

TEST_F(SaQueue2Test, ReceiveExRefTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.receive_ex(val, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, ReceiveExRefChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.receive_ex(val, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, ReceiveExOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_ex(10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, ReceiveExOptionalWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_ex(0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_empty);
}

TEST_F(SaQueue2Test, ReceiveExOptionalTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_ex(100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, ReceiveExOptionalChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive_ex(std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// ================================================================
// RECEIVE CHRONO OPTIONAL (lines 446-447)
// ================================================================

TEST_F(SaQueue2Test, ReceiveChronoOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive(std::chrono::milliseconds(50));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, ReceiveChronoOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.receive(std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
}

// ================================================================
// PEEK / PEEK_ISR (lines 552-553, 565-567, 596-601, 611-616, 581-586, 627-637)
// ================================================================

TEST_F(SaQueue2Test, PeekSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek(val, 10);
  EXPECT_EQ(r, pdPASS);
}

TEST_F(SaQueue2Test, PeekChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek(val, std::chrono::milliseconds(50));
  EXPECT_EQ(r, pdPASS);
}

TEST_F(SaQueue2Test, PeekOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.peek(10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, PeekOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.peek(0);
  EXPECT_FALSE(r.has_value());
}

TEST_F(SaQueue2Test, PeekOptionalChronoSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.peek(std::chrono::milliseconds(50));
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, PeekIsrRefSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_isr(val);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaQueue2Test, PeekIsrRefFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_isr(val);
  EXPECT_EQ(r.result, pdFALSE);
}

TEST_F(SaQueue2Test, PeekIsrOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.peek_isr();
  EXPECT_TRUE(r.result.has_value());
}

TEST_F(SaQueue2Test, PeekIsrOptionalEmpty) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeekFromISR(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.peek_isr();
  EXPECT_FALSE(r.result.has_value());
}

// ================================================================
// PEEK_EX (lines 790, 792-794, 796, 798)
// ================================================================

TEST_F(SaQueue2Test, PeekExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 10)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_ex(val, 10);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, PeekExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_ex(val, 0);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_empty);
}

TEST_F(SaQueue2Test, PeekExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_ex(val, 100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(SaQueue2Test, PeekExChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueuePeek(h, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  auto r = q.peek_ex(val, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// ================================================================
// RESET / RESET_EX (line 781)
// ================================================================

TEST_F(SaQueue2Test, ResetSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReset(h)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.reset(), pdPASS);
}

TEST_F(SaQueue2Test, ResetExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReset(h)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// ================================================================
// OVERWRITE / OVERWRITE_ISR / OVERWRITE_EX (lines 786, 788)
// ================================================================

TEST_F(SaQueue2Test, OverwriteSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwrite(h, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.overwrite(42), pdPASS);
}

TEST_F(SaQueue2Test, OverwriteIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwriteFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.overwrite_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaQueue2Test, OverwriteExSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwrite(h, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.overwrite_ex(42);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SaQueue2Test, OverwriteExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueOverwrite(h, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.overwrite_ex(42);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::queue_full);
}

// ================================================================
// ACCESSORS: messages_waiting, messages_waiting_isr, spaces_available,
//            full_isr, empty_isr, is_empty, is_full, name
// ================================================================

TEST_F(SaQueue2Test, MessagesWaiting) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, uxQueueMessagesWaiting(h)).WillOnce(Return(3));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.messages_waiting(), 3);
}

TEST_F(SaQueue2Test, MessagesWaitingIsr) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, uxQueueMessagesWaitingFromISR(h)).WillOnce(Return(5));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.messages_waiting_isr(), 5);
}

TEST_F(SaQueue2Test, SpacesAvailable) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, uxQueueSpacesAvailable(h)).WillOnce(Return(7));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.spaces_available(), 7);
}

TEST_F(SaQueue2Test, FullIsr) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueFullFromISR(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.full_isr(), pdTRUE);
}

TEST_F(SaQueue2Test, FullIsrNotFull) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueFullFromISR(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.full_isr(), pdFALSE);
}

TEST_F(SaQueue2Test, EmptyIsr) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueEmptyFromISR(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.empty_isr(), pdTRUE);
}

TEST_F(SaQueue2Test, EmptyIsrNotEmpty) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueIsQueueEmptyFromISR(h)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.empty_isr(), pdFALSE);
}

TEST_F(SaQueue2Test, NameMethod) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vQueueAddToRegistry(h, _));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return("my_q"));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return("my_q"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(h));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q("my_q");
  EXPECT_EQ(q.name(), std::string("my_q"));
}

// ================================================================
// SEND_BACK_ISR / SEND_FRONT_ISR direct calls
// ================================================================

TEST_F(SaQueue2Test, SendBackIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_back_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

TEST_F(SaQueue2Test, SendFrontIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  auto r = q.send_front_isr(42);
  EXPECT_EQ(r.result, pdPASS);
}

// ================================================================
// SEND / RECEIVE with chrono overloads (template instantiation)
// ================================================================

TEST_F(SaQueue2Test, SendChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.send(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(SaQueue2Test, SendBackChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToBack(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.send_back(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(SaQueue2Test, SendFrontChrono) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSendToFront(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  EXPECT_EQ(q.send_front(42, std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(SaQueue2Test, ReceiveChronoRef) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueReceive(h, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  Q q;
  int val;
  EXPECT_EQ(q.receive(val, std::chrono::milliseconds(100)), pdPASS);
}

// ================================================================
// STATIC_QUEUE_ALLOCATOR SWAP (lines 69, 71-73) via two swaps
// ================================================================

TEST_F(SaQueue2Test, SwapWithNamedQueues) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x9011);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0x9012);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, vQueueAddToRegistry(h1, _));
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vQueueAddToRegistry(h2, _));
  EXPECT_CALL(*mock, pcQueueGetName(h1)).WillOnce(Return("q1"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(h1));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  EXPECT_CALL(*mock, pcQueueGetName(h2)).WillOnce(Return("q2"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(h2));
  EXPECT_CALL(*mock, vQueueDelete(h2));
  Q q1("q1");
  Q q2("q2");
  q1.swap(q2);
}
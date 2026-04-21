#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/freertos_mocks.hpp"
#include "freertos_event_group.hpp"
#include "freertos_isr_result.hpp"

using ::testing::Return;
using ::testing::_;

class EventGroupCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
    mock_handle = reinterpret_cast<EventGroupHandle_t>(0xABCDEF00);
  }
  void TearDown() override { FreeRTOSMockInstance::resetInstance(); }
  std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;
  EventGroupHandle_t mock_handle;
};

TEST_F(EventGroupCoverageTest, MoveConstructWithAllocator) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  freertos::sa::event_group eg2(std::move(eg));
  EXPECT_EQ(eg2.handle(), mock_handle);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, MoveAssignment) {
  auto handle2 = reinterpret_cast<EventGroupHandle_t>(0x11);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_))
      .WillOnce(Return(mock_handle))
      .WillOnce(Return(handle2));
  freertos::sa::event_group eg1;
  freertos::sa::event_group eg2;
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle2));
  eg1 = std::move(eg2);
}

TEST_F(EventGroupCoverageTest, Swap) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_))
      .WillOnce(Return(mock_handle))
      .WillOnce(Return(reinterpret_cast<EventGroupHandle_t>(0x22)));
  freertos::sa::event_group eg1;
  freertos::sa::event_group eg2;
  eg1.swap(eg2);
  EXPECT_EQ(eg2.handle(), mock_handle);
  EXPECT_CALL(*mock, vEventGroupDelete(_)).Times(2);
}

TEST_F(EventGroupCoverageTest, FriendSwap) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_))
      .WillOnce(Return(mock_handle))
      .WillOnce(Return(reinterpret_cast<EventGroupHandle_t>(0x33)));
  freertos::sa::event_group eg1;
  freertos::sa::event_group eg2;
  using std::swap;
  swap(eg1, eg2);
  EXPECT_CALL(*mock, vEventGroupDelete(_)).Times(2);
}

TEST_F(EventGroupCoverageTest, SetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(_, _, _))
      .WillOnce(Return(pdPASS));
  auto result = eg.set_bits_isr(0x01);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SetBitsExIsrSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(_, _, _))
      .WillOnce(Return(pdPASS));
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SetBitsExIsrFailure) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(_, _, _))
      .WillOnce(Return(pdFAIL));
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, WaitBitsExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupWaitBits(_, _, _, _, _))
      .WillOnce(Return(0x00));
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, WaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupWaitBits(_, _, _, _, _))
      .WillOnce(Return(0x00));
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, WaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupWaitBits(_, _, _, _, _))
      .WillOnce(Return(0x01));
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, WaitBitsExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupWaitBits(_, _, _, _, _))
      .WillOnce(Return(0x01));
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SyncExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSync(_, _, _, _))
      .WillOnce(Return(0x00));
  auto result = eg.sync_ex(0x01, 0x03, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSync(_, _, _, _))
      .WillOnce(Return(0x01));
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SyncExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSync(_, _, _, _))
      .WillOnce(Return(0x03));
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SyncExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSync(_, _, _, _))
      .WillOnce(Return(0x03));
  auto result = eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(50));
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, ClearBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupClearBits(_, _)).WillOnce(Return(0x03));
  auto result = eg.clear_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, GetBits) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupGetBits(mock_handle)).WillOnce(Return(0x05));
  EXPECT_EQ(eg.get_bits(), 0x05);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, GetBitsIsr) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupGetBitsFromISR(mock_handle))
      .WillOnce(Return(0x07));
  EXPECT_EQ(eg.get_bits_isr(), 0x07);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SetBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSetBits(_, _)).WillOnce(Return(0x01));
  auto result = eg.set_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, SyncChrono) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  EXPECT_CALL(*mock, xEventGroupSync(_, _, _, _)).WillOnce(Return(0x03));
  auto result = eg.sync(0x01, 0x03, std::chrono::milliseconds(50));
  EXPECT_EQ(result, 0x03);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}

TEST_F(EventGroupCoverageTest, StaticAllocatorSwap) {
  freertos::static_event_group_allocator a1;
  freertos::static_event_group_allocator a2;
  a1.swap(a2);
}

TEST_F(EventGroupCoverageTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(mock_handle));
  freertos::sa::event_group eg;
  auto *ptr = &eg;
  eg = std::move(*ptr);
  EXPECT_EQ(eg.handle(), mock_handle);
  EXPECT_CALL(*mock, vEventGroupDelete(mock_handle));
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class MockRegion {
public:
  int call_count = 0;
  int fail_after = -1;
  void *allocate(size_t) noexcept {
    call_count++;
    if (fail_after >= 0 && call_count > fail_after) {
      return nullptr;
    }
    return reinterpret_cast<void *>(0x1000 + call_count * 0x100);
  }
  void deallocate(void *) noexcept {}
};

class TestRegion {
public:
  int call_count = 0;
  int fail_after = -1;
  void *allocate(size_t) {
    call_count++;
    if (fail_after >= 0 && call_count > fail_after) {
      return nullptr;
    }
    return reinterpret_cast<void *>(0x2000 + call_count * 0x100);
  }
  void deallocate(void *) {}
};

class ExternalAllocator4Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// ================================================================
// external_semaphore_allocator<MockRegion>
// Lines 87, 88, 100, 106, 122, 130
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_Construct) {
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_MoveConstruct) {
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc1(region);
  freertos::external_semaphore_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_MoveConstructWithMemory) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA001);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc1(region);
  alloc1.create_binary();
  freertos::external_semaphore_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_DestructWithMemory) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA002);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_semaphore_allocator<MockRegion> alloc(region);
    alloc.create_mutex();
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_CreateBinary) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA010);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_binary();
  EXPECT_NE(result, nullptr);
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_CreateMutex) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA011);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_mutex();
  EXPECT_NE(result, nullptr);
}

TEST_F(ExternalAllocator4Test, MockRegion_Semaphore_CreateRecursiveMutex) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA012);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_semaphore_allocator<MockRegion> alloc(region);
  auto result = alloc.create_recursive_mutex();
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_queue_allocator<MockRegion, 10ul, int>
// Lines 161, 162, 177, 186
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_Queue_Construct) {
  MockRegion region;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_Queue_MoveConstruct) {
  MockRegion region;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc1(region);
  freertos::external_queue_allocator<MockRegion, 10, int> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Queue_MoveConstructWithMemory) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xB001);
  EXPECT_CALL(*mock, xQueueCreateStatic(10, sizeof(int), _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc1(region);
  alloc1.create();
  freertos::external_queue_allocator<MockRegion, 10, int> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Queue_DestructWithMemory) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xB002);
  EXPECT_CALL(*mock, xQueueCreateStatic(10, sizeof(int), _, _)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
    alloc.create();
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_Queue_Create) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xB010);
  EXPECT_CALL(*mock, xQueueCreateStatic(10, sizeof(int), _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_queue_allocator<MockRegion, 10, int> alloc(region);
  auto result = alloc.create();
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_event_group_allocator<MockRegion>
// Lines 222, 223, 236, 242
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_EventGroup_Construct) {
  MockRegion region;
  freertos::external_event_group_allocator<MockRegion> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_EventGroup_MoveConstruct) {
  MockRegion region;
  freertos::external_event_group_allocator<MockRegion> alloc1(region);
  freertos::external_event_group_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_EventGroup_MoveConstructWithMemory) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xC001);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_event_group_allocator<MockRegion> alloc1(region);
  alloc1.create();
  freertos::external_event_group_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_EventGroup_DestructWithMemory) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xC002);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_event_group_allocator<MockRegion> alloc(region);
    alloc.create();
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_EventGroup_Create) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xC010);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_event_group_allocator<MockRegion> alloc(region);
  auto result = alloc.create();
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_stream_buffer_allocator<MockRegion, 64ul>
// Lines 273, 275, 291, 300
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_StreamBuffer_Construct) {
  MockRegion region;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_StreamBuffer_MoveConstruct) {
  MockRegion region;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc1(region);
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_StreamBuffer_MoveConstructWithMemory) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xD001);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc1(region);
  alloc1.create(1);
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_StreamBuffer_DestructWithMemory) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xD002);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
    alloc.create(1);
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_StreamBuffer_Create) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xD010);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_stream_buffer_allocator<MockRegion, 64> alloc(region);
  auto result = alloc.create(1);
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_message_buffer_allocator<MockRegion, 256ul>
// Lines 340, 342, 358, 367
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_MessageBuffer_Construct) {
  MockRegion region;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_MessageBuffer_MoveConstruct) {
  MockRegion region;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc1(region);
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_MessageBuffer_MoveConstructWithMemory) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xE001);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc1(region);
  alloc1.create();
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_MessageBuffer_DestructWithMemory) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xE002);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, _, _)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
    alloc.create();
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_MessageBuffer_Create) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xE010);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_message_buffer_allocator<MockRegion, 256> alloc(region);
  auto result = alloc.create();
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_sw_timer_allocator<MockRegion>
// Lines 402, 403, 415, 421
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_SwTimer_Construct) {
  MockRegion region;
  freertos::external_sw_timer_allocator<MockRegion> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_SwTimer_MoveConstruct) {
  MockRegion region;
  freertos::external_sw_timer_allocator<MockRegion> alloc1(region);
  freertos::external_sw_timer_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_SwTimer_MoveConstructWithMemory) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xF001);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_sw_timer_allocator<MockRegion> alloc1(region);
  alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
  freertos::external_sw_timer_allocator<MockRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_SwTimer_DestructWithMemory) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xF002);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_sw_timer_allocator<MockRegion> alloc(region);
    alloc.create("t", 100, pdTRUE, nullptr, nullptr);
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_SwTimer_Create) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xF010);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_sw_timer_allocator<MockRegion> alloc(region);
  auto result = alloc.create("timer", 200, pdFALSE, nullptr, nullptr);
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_task_allocator<MockRegion, 512ul>
// Lines 451, 452, 467, 476
// ================================================================

TEST_F(ExternalAllocator4Test, MockRegion_Task_Construct) {
  MockRegion region;
  freertos::external_task_allocator<MockRegion, 512> alloc(region);
}

TEST_F(ExternalAllocator4Test, MockRegion_Task_MoveConstruct) {
  MockRegion region;
  freertos::external_task_allocator<MockRegion, 512> alloc1(region);
  freertos::external_task_allocator<MockRegion, 512> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Task_MoveConstructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x100001);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_task_allocator<MockRegion, 512> alloc1(region);
  alloc1.create(nullptr, "task", 1, nullptr);
  freertos::external_task_allocator<MockRegion, 512> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, MockRegion_Task_DestructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x100002);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  {
    freertos::external_task_allocator<MockRegion, 512> alloc(region);
    alloc.create(nullptr, "task", 1, nullptr);
  }
}

TEST_F(ExternalAllocator4Test, MockRegion_Task_Create) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x100010);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  MockRegion region;
  freertos::external_task_allocator<MockRegion, 512> alloc(region);
  auto result = alloc.create(nullptr, "task", 2, nullptr);
  EXPECT_NE(result, nullptr);
}

// ================================================================
// external_task_allocator<TestRegion, 512ul>
// Lines 451, 452, 467, 476
// ================================================================

TEST_F(ExternalAllocator4Test, TestRegion_Task_Construct) {
  TestRegion region;
  freertos::external_task_allocator<TestRegion, 512> alloc(region);
}

TEST_F(ExternalAllocator4Test, TestRegion_Task_MoveConstruct) {
  TestRegion region;
  freertos::external_task_allocator<TestRegion, 512> alloc1(region);
  freertos::external_task_allocator<TestRegion, 512> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, TestRegion_Task_MoveConstructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x200001);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  TestRegion region;
  freertos::external_task_allocator<TestRegion, 512> alloc1(region);
  alloc1.create(nullptr, "task", 1, nullptr);
  freertos::external_task_allocator<TestRegion, 512> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocator4Test, TestRegion_Task_DestructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x200002);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  TestRegion region;
  {
    freertos::external_task_allocator<TestRegion, 512> alloc(region);
    alloc.create(nullptr, "task", 1, nullptr);
  }
}

TEST_F(ExternalAllocator4Test, TestRegion_Task_Create) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x200010);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  TestRegion region;
  freertos::external_task_allocator<TestRegion, 512> alloc(region);
  auto result = alloc.create(nullptr, "task", 2, nullptr);
  EXPECT_NE(result, nullptr);
}
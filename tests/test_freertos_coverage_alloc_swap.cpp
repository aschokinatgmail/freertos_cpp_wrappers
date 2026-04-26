#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

struct TestMemoryRegion {
  void *allocate(size_t size) { return std::malloc(size); }
  void deallocate(void *ptr) { std::free(ptr); }
};

class AllocSwapTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TestMemoryRegion region;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// ==================== SEMAPHORE ALLOCATOR ====================

TEST_F(AllocSwapTest, SemaphoreSwapBothWithMemory) {
  SemaphoreHandle_t h1 = reinterpret_cast<SemaphoreHandle_t>(0x1001);
  SemaphoreHandle_t h2 = reinterpret_cast<SemaphoreHandle_t>(0x1002);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h2));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create_binary();
    alloc2.create_mutex();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, SemaphoreSwapOneWithMemoryOneEmpty) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1010);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create_binary();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, SemaphoreSwapBothEmpty) {
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, SemaphoreMoveConstructWithMemory) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1020);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create_mutex();
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, SemaphoreMoveConstructWithoutMemory) {
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, SemaphoreMoveThenDestructNoDoubleFree) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x1030);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create_binary();
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, SemaphoreDefaultConstructDestruct) {
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
}

// ==================== QUEUE ALLOCATOR ====================

TEST_F(AllocSwapTest, QueueSwapBothWithMemory) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0x2001);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0x2002);
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), _, _))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, QueueSwapOneWithMemoryOneEmpty) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x2010);
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), _, _))
      .WillOnce(Return(h));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, QueueSwapBothEmpty) {
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, QueueMoveConstructWithMemory) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x2020);
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), _, _))
      .WillOnce(Return(h));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    alloc1.create();
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, QueueMoveConstructWithoutMemory) {
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, QueueMoveThenDestructNoDoubleFree) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x2030);
  EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(int), _, _))
      .WillOnce(Return(h));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    alloc1.create();
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, QueueDefaultConstructDestruct) {
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc(region);
}

// ==================== EVENT GROUP ALLOCATOR ====================

TEST_F(AllocSwapTest, EventGroupSwapBothWithMemory) {
  EventGroupHandle_t h1 = reinterpret_cast<EventGroupHandle_t>(0x3001);
  EventGroupHandle_t h2 = reinterpret_cast<EventGroupHandle_t>(0x3002);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, EventGroupSwapOneWithMemoryOneEmpty) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x3010);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, EventGroupSwapBothEmpty) {
  freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, EventGroupMoveConstructWithMemory) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x3020);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create();
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, EventGroupMoveConstructWithoutMemory) {
  freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_event_group_allocator<TestMemoryRegion> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, EventGroupMoveThenDestructNoDoubleFree) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x3030);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create();
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, EventGroupDefaultConstructDestruct) {
  freertos::external_event_group_allocator<TestMemoryRegion> alloc(region);
}

// ==================== STREAM BUFFER ALLOCATOR ====================

TEST_F(AllocSwapTest, StreamBufferSwapBothWithMemory) {
  StreamBufferHandle_t h1 = reinterpret_cast<StreamBufferHandle_t>(0x4001);
  StreamBufferHandle_t h2 = reinterpret_cast<StreamBufferHandle_t>(0x4002);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
        region);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
        region);
    alloc1.create(1);
    alloc2.create(2);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, StreamBufferSwapOneWithMemoryOneEmpty) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x4010);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
        region);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
        region);
    alloc1.create(1);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, StreamBufferSwapBothEmpty) {
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
      region);
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
      region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, StreamBufferMoveConstructWithMemory) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x4020);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
        region);
    alloc1.create(1);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, StreamBufferMoveConstructWithoutMemory) {
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
      region);
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, StreamBufferMoveThenDestructNoDoubleFree) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x4030);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(
        region);
    alloc1.create(1);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, StreamBufferDefaultConstructDestruct) {
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc(region);
}

// ==================== MESSAGE BUFFER ALLOCATOR ====================

TEST_F(AllocSwapTest, MessageBufferSwapBothWithMemory) {
  MessageBufferHandle_t h1 = reinterpret_cast<MessageBufferHandle_t>(0x5001);
  MessageBufferHandle_t h2 = reinterpret_cast<MessageBufferHandle_t>(0x5002);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(128, _, _))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
        region);
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
        region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, MessageBufferSwapOneWithMemoryOneEmpty) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x5010);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(128, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
        region);
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
        region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, MessageBufferSwapBothEmpty) {
  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
      region);
  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
      region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, MessageBufferMoveConstructWithMemory) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x5020);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(128, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
        region);
    alloc1.create();
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, MessageBufferMoveConstructWithoutMemory) {
  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
      region);
  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, MessageBufferMoveThenDestructNoDoubleFree) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x5030);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(128, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc1(
        region);
    alloc1.create();
    freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, MessageBufferDefaultConstructDestruct) {
  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc(
      region);
}

// ==================== SW TIMER ALLOCATOR ====================

TEST_F(AllocSwapTest, SwTimerSwapBothWithMemory) {
  TimerHandle_t h1 = reinterpret_cast<TimerHandle_t>(0x6001);
  TimerHandle_t h2 = reinterpret_cast<TimerHandle_t>(0x6002);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create("t1", 100, pdTRUE, nullptr, nullptr);
    alloc2.create("t2", 200, pdFALSE, nullptr, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, SwTimerSwapOneWithMemoryOneEmpty) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x6010);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, SwTimerSwapBothEmpty) {
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, SwTimerMoveConstructWithMemory) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x6020);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, SwTimerMoveConstructWithoutMemory) {
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, SwTimerMoveThenDestructNoDoubleFree) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0x6030);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, SwTimerDefaultConstructDestruct) {
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(region);
}

// ==================== TASK ALLOCATOR ====================

TEST_F(AllocSwapTest, TaskSwapBothWithMemory) {
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x7001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x7002);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
    alloc1.create(nullptr, "task1", 1, nullptr);
    alloc2.create(nullptr, "task2", 2, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, TaskSwapOneWithMemoryOneEmpty) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x7010);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
    alloc1.create(nullptr, "task", 1, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(AllocSwapTest, TaskSwapBothEmpty) {
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
  alloc1.swap(alloc2);
}

TEST_F(AllocSwapTest, TaskMoveConstructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x7020);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    alloc1.create(nullptr, "task", 1, nullptr);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, TaskMoveConstructWithoutMemory) {
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(
      std::move(alloc1));
}

TEST_F(AllocSwapTest, TaskMoveThenDestructNoDoubleFree) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x7030);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(h));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    alloc1.create(nullptr, "task", 1, nullptr);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(
        std::move(alloc1));
  }
}

TEST_F(AllocSwapTest, TaskDefaultConstructDestruct) {
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc(region);
}
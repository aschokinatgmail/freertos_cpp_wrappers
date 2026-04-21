#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

using TestMemoryRegion = freertos::external_memory_region;

static void *test_allocate(size_t size) { return malloc(size); }
static void test_deallocate(void *ptr) { free(ptr); }

static int g_alloc_seq = 0;
static void *test_allocate_second_fails(size_t size) {
  g_alloc_seq++;
  if (g_alloc_seq == 1) {
    return malloc(size);
  }
  return nullptr;
}

class ExternalAllocatorCoverage3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TestMemoryRegion region{test_allocate, test_deallocate};

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    g_alloc_seq = 0;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// --- Default-construct then destruct (no create() called) ---

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreDefaultConstructDestruct) {
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, QueueDefaultConstructDestruct) {
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupDefaultConstructDestruct) {
  freertos::external_event_group_allocator<TestMemoryRegion> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferDefaultConstructDestruct) {
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferDefaultConstructDestruct) {
  freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerDefaultConstructDestruct) {
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(region);
}

TEST_F(ExternalAllocatorCoverage3Test, TaskDefaultConstructDestruct) {
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc(region);
}

// --- Move constructors with allocated memory ---

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreMoveConstructWithMemory) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xA001);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
  alloc1.create_mutex();
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, QueueMoveConstructWithMemory) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xA002);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h));
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
  alloc1.create();
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupMoveConstructWithMemory) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xA003);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
  alloc1.create();
  freertos::external_event_group_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferMoveConstructWithMemory) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xA004);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
  alloc1.create(1);
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferMoveConstructWithMemory) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xA005);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
  alloc1.create();
  freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerMoveConstructWithMemory) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xA006);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
  alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
}

TEST_F(ExternalAllocatorCoverage3Test, TaskMoveConstructWithMemory) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0xA007);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
  alloc1.create(nullptr, "task", 1, nullptr);
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(std::move(alloc1));
}

// --- Move-then-destruct (moved-from should not double-free) ---

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreMoveThenDestructBoth) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xB001);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create_mutex();
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, QueueMoveThenDestructBoth) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xB002);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    alloc1.create();
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupMoveThenDestructBoth) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xB003);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create();
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferMoveThenDestructBoth) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xB004);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    alloc1.create(1);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferMoveThenDestructBoth) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xB005);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    alloc1.create();
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerMoveThenDestructBoth) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xB006);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(std::move(alloc1));
  }
}

TEST_F(ExternalAllocatorCoverage3Test, TaskMoveThenDestructBoth) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0xB007);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    alloc1.create(nullptr, "task", 1, nullptr);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(std::move(alloc1));
  }
}

// --- swap() methods for all 7 allocator types ---

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreSwapWithMemory) {
  SemaphoreHandle_t h1 = reinterpret_cast<SemaphoreHandle_t>(0xC001);
  SemaphoreHandle_t h2 = reinterpret_cast<SemaphoreHandle_t>(0xC002);
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

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreSwapEmptyWithEmpty) {
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, QueueSwapWithMemory) {
  QueueHandle_t h1 = reinterpret_cast<QueueHandle_t>(0xC010);
  QueueHandle_t h2 = reinterpret_cast<QueueHandle_t>(0xC011);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, QueueSwapEmptyWithEmpty) {
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupSwapWithMemory) {
  EventGroupHandle_t h1 = reinterpret_cast<EventGroupHandle_t>(0xC020);
  EventGroupHandle_t h2 = reinterpret_cast<EventGroupHandle_t>(0xC021);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupSwapEmptyWithEmpty) {
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferSwapWithMemory) {
  StreamBufferHandle_t h1 = reinterpret_cast<StreamBufferHandle_t>(0xC030);
  StreamBufferHandle_t h2 = reinterpret_cast<StreamBufferHandle_t>(0xC031);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.create(1);
    alloc2.create(2);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferSwapEmptyWithEmpty) {
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferSwapWithMemory) {
  MessageBufferHandle_t h1 = reinterpret_cast<MessageBufferHandle_t>(0xC040);
  MessageBufferHandle_t h2 = reinterpret_cast<MessageBufferHandle_t>(0xC041);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.create();
    alloc2.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferSwapEmptyWithEmpty) {
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerSwapWithMemory) {
  TimerHandle_t h1 = reinterpret_cast<TimerHandle_t>(0xC050);
  TimerHandle_t h2 = reinterpret_cast<TimerHandle_t>(0xC051);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create("t1", 100, pdTRUE, nullptr, nullptr);
    alloc2.create("t2", 200, pdFALSE, nullptr, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerSwapEmptyWithEmpty) {
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, TaskSwapWithMemory) {
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0xC060);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0xC061);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h1)).WillOnce(Return(h2));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
    alloc1.create(nullptr, "task1", 1, nullptr);
    alloc2.create(nullptr, "task2", 2, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, TaskSwapEmptyWithEmpty) {
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
    alloc1.swap(alloc2);
  }
}

// --- Swap one-with-memory and one-empty ---

TEST_F(ExternalAllocatorCoverage3Test, SemaphoreSwapOneHasMemoryOneEmpty) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0xC070);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create_binary();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, QueueSwapOneHasMemoryOneEmpty) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xC071);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h));
  {
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc1(region);
    freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc2(region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, EventGroupSwapOneHasMemoryOneEmpty) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0xC072);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  {
    freertos::external_event_group_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_event_group_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, StreamBufferSwapOneHasMemoryOneEmpty) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0xC073);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.create(1);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, MessageBufferSwapOneHasMemoryOneEmpty) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0xC074);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  {
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc1(region);
    freertos::external_message_buffer_allocator<TestMemoryRegion, 64> alloc2(region);
    alloc1.create();
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, SwTimerSwapOneHasMemoryOneEmpty) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xC075);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc1(region);
    freertos::external_sw_timer_allocator<TestMemoryRegion> alloc2(region);
    alloc1.create("t", 100, pdTRUE, nullptr, nullptr);
    alloc1.swap(alloc2);
  }
}

TEST_F(ExternalAllocatorCoverage3Test, TaskSwapOneHasMemoryOneEmpty) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0xC076);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  {
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc1(region);
    freertos::external_task_allocator<TestMemoryRegion, 256> alloc2(region);
    alloc1.create(nullptr, "task", 1, nullptr);
    alloc1.swap(alloc2);
  }
}

// --- Task allocator partial failure (stack alloc fails) ---

TEST_F(ExternalAllocatorCoverage3Test, TaskAllocatorStackAllocFails) {
  TestMemoryRegion partial_region{test_allocate_second_fails, test_deallocate};
  g_alloc_seq = 0;
  freertos::external_task_allocator<TestMemoryRegion, 256> alloc(partial_region);
  auto result = alloc.create(nullptr, "task", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

// --- Queue allocator partial failure via region (struct alloc succeeds, storage fails) ---

TEST_F(ExternalAllocatorCoverage3Test, QueueAllocatorStorageAllocFails) {
  TestMemoryRegion partial_region{test_allocate_second_fails, test_deallocate};
  g_alloc_seq = 0;
  freertos::external_queue_allocator<TestMemoryRegion, 4, int> alloc(partial_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}
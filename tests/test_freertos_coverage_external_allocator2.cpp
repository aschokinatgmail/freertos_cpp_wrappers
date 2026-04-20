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
static void *test_allocate_fail(size_t) { return nullptr; }

static int g_alloc_count = 0;
static void *test_allocate_partial(size_t size) {
  g_alloc_count++;
  if (g_alloc_count == 1) {
    return malloc(size);
  }
  return nullptr;
}

class ExternalAllocatorCoverageTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TestMemoryRegion region{test_allocate, test_deallocate};
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};
  TestMemoryRegion partial_region{test_allocate_partial, test_deallocate};

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    g_alloc_count = 0;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExternalAllocatorCoverageTest, ExternalSemaphoreAllocatorDestructorFreesAllocated) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x3001);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<freertos::external_semaphore_allocator<TestMemoryRegion>> sem(region);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceBinarySemaphore) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x4001);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::ea::binary_semaphore<TestMemoryRegion> sem(region);
  EXPECT_EQ(sem.give(), pdTRUE);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceCountingSemaphore) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x4002);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, 3, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::ea::counting_semaphore<TestMemoryRegion> sem(3, region);
  EXPECT_EQ(sem.give(), pdTRUE);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceMutex) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x4003);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::ea::mutex<TestMemoryRegion> m(region);
  EXPECT_EQ(m.try_lock(), pdTRUE);
  m.unlock();
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceRecursiveMutex) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x4004);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::ea::recursive_mutex<TestMemoryRegion> m(region);
  EXPECT_EQ(m.try_lock(), pdTRUE);
  m.unlock();
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceQueue) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0x5001);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xQueueSend(h, _, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::ea::queue<TestMemoryRegion, 5, int> q(nullptr, region);
  EXPECT_EQ(q.send(42, portMAX_DELAY), pdPASS);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceStreamBuffer) {
  StreamBufferHandle_t h = reinterpret_cast<StreamBufferHandle_t>(0x6001);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xStreamBufferSend(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(h));
  freertos::ea::stream_buffer<TestMemoryRegion, 256> sb(1, region);
  EXPECT_EQ(sb.send("test", 4, portMAX_DELAY), 4u);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceMessageBuffer) {
  MessageBufferHandle_t h = reinterpret_cast<MessageBufferHandle_t>(0x7001);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xMessageBufferSend(h, _, 4, portMAX_DELAY)).WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(h));
  freertos::ea::message_buffer<TestMemoryRegion, 256> mb(region);
  EXPECT_EQ(mb.send("test", 4, portMAX_DELAY), 4u);
}

TEST_F(ExternalAllocatorCoverageTest, EaNamespaceEventGroup) {
  EventGroupHandle_t h = reinterpret_cast<EventGroupHandle_t>(0x8001);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xEventGroupSetBits(h, 0x01)).WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(h));
  freertos::ea::event_group<TestMemoryRegion> eg(region);
  EXPECT_EQ(eg.set_bits(0x01), 0x01);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalSemaphoreAllocatorAllCreateMethods) {
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5001);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(2, 2, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(h));
  {
    freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
    auto bh = alloc.create_binary();
    EXPECT_EQ(bh, h);
    auto ch = alloc.create_counting(2);
    EXPECT_EQ(ch, h);
    auto mh = alloc.create_mutex();
    EXPECT_EQ(mh, h);
    auto rh = alloc.create_recursive_mutex();
    EXPECT_EQ(rh, h);
  }
}

TEST_F(ExternalAllocatorCoverageTest, ExternalAllocatorFailedAllocationNullptr) {
  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create_binary();
  EXPECT_EQ(result, nullptr);
  auto result2 = alloc.create_counting(5);
  EXPECT_EQ(result2, nullptr);
  auto result3 = alloc.create_mutex();
  EXPECT_EQ(result3, nullptr);
  auto result4 = alloc.create_recursive_mutex();
  EXPECT_EQ(result4, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalEventGroupAllocatorFailedAllocation) {
  freertos::external_event_group_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalStreamBufferAllocatorFailedAllocation) {
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 256> alloc(fail_region);
  auto result = alloc.create(1);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalMessageBufferAllocatorFailedAllocation) {
  freertos::external_message_buffer_allocator<TestMemoryRegion, 256> alloc(fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalSwTimerAllocatorFailedAllocation) {
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create("timer", 100, pdTRUE, nullptr, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalQueueAllocatorFailedAllocations) {
  freertos::external_queue_allocator<TestMemoryRegion, 10, int> alloc(fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalQueueAllocatorPartialFailure) {
  g_alloc_count = 0;
  freertos::external_queue_allocator<TestMemoryRegion, 10, int> alloc(partial_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalStreamBufferAllocatorPartialFailure) {
  g_alloc_count = 0;
  freertos::external_stream_buffer_allocator<TestMemoryRegion, 256> alloc(partial_region);
  auto result = alloc.create(1);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalMessageBufferAllocatorPartialFailure) {
  g_alloc_count = 0;
  freertos::external_message_buffer_allocator<TestMemoryRegion, 256> alloc(partial_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalTaskAllocatorPartialFailure) {
  g_alloc_count = 0;
  freertos::external_task_allocator<TestMemoryRegion, 512> alloc(partial_region);
  auto result = alloc.create(nullptr, "task", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalSwTimerAllocatorCreate) {
  TimerHandle_t h = reinterpret_cast<TimerHandle_t>(0xC001);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(h));
  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create("timer", 100, pdTRUE, nullptr, nullptr);
  EXPECT_EQ(result, h);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalTaskAllocatorCreate) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0xD001);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(h));
  freertos::external_task_allocator<TestMemoryRegion, 512> alloc(region);
  auto result = alloc.create(nullptr, "task", 1, nullptr);
  EXPECT_EQ(result, h);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalQueueAllocatorDestructorNoLeak) {
  QueueHandle_t h = reinterpret_cast<QueueHandle_t>(0xE001);
  EXPECT_CALL(*mock, xQueueCreateStatic(_, sizeof(int), _, _)).WillOnce(Return(h));
  EXPECT_CALL(*mock, pcQueueGetName(h)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h));
  freertos::queue<10, int, freertos::external_queue_allocator<TestMemoryRegion, 10, int>> q(nullptr, region);
}
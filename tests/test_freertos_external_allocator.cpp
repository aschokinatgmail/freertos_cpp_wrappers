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

class ExternalAllocatorTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    region.allocate = test_allocate;
    region.deallocate = test_deallocate;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  TestMemoryRegion region;
};

TEST_F(ExternalAllocatorTest, ExternalMemoryRegionStructWorks) {
  freertos::external_memory_region r{test_allocate, test_deallocate};
  void *mem = r.allocate(64);
  ASSERT_NE(mem, nullptr);
  r.deallocate(mem);
}

TEST_F(ExternalAllocatorTest, ExternalMemoryRegionFailedAllocation) {
  freertos::external_memory_region r{test_allocate_fail, test_deallocate};
  void *mem = r.allocate(64);
  ASSERT_EQ(mem, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorCreateBinary) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xDEADBEEF);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(handle));

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create_binary();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorCreateMutex) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xBEEFCAFE);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(handle));

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create_mutex();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorCreateCounting) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xCAFEBABE);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(handle));

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create_counting(5);
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorCreateRecursiveMutex) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0x12345678);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(handle));

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create_recursive_mutex();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorFailedAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create_binary();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalSemaphoreAllocatorMoveConstruct) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAABBBB);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(handle));

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc1(region);
  alloc1.create_mutex();

  freertos::external_semaphore_allocator<TestMemoryRegion> alloc2(
      std::move(alloc1));
}

TEST_F(ExternalAllocatorTest, ExternalEventGroupAllocatorCreate) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEEEEEE);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(handle));

  freertos::external_event_group_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalEventGroupAllocatorFailedAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_event_group_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalQueueAllocatorCreate) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0x11111111);
  EXPECT_CALL(*mock, xQueueCreateStatic(10, sizeof(int), _, _))
      .WillOnce(Return(handle));

  freertos::external_queue_allocator<TestMemoryRegion, 10, int> alloc(region);
  auto result = alloc.create();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalQueueAllocatorFailedStructAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_queue_allocator<TestMemoryRegion, 10, int> alloc(
      fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalStreamBufferAllocatorCreate) {
  StreamBufferHandle_t handle =
      reinterpret_cast<StreamBufferHandle_t>(0x22222222);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, _, _))
      .WillOnce(Return(handle));

  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc(
      region);
  auto result = alloc.create(1);
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalStreamBufferAllocatorFailedAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_stream_buffer_allocator<TestMemoryRegion, 64> alloc(
      fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalMessageBufferAllocatorCreate) {
  MessageBufferHandle_t handle =
      reinterpret_cast<MessageBufferHandle_t>(0x33333333);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(128, _, _))
      .WillOnce(Return(handle));

  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc(
      region);
  auto result = alloc.create();
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalMessageBufferAllocatorFailedAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_message_buffer_allocator<TestMemoryRegion, 128> alloc(
      fail_region);
  auto result = alloc.create();
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalSwTimerAllocatorCreate) {
  TimerHandle_t handle = reinterpret_cast<TimerHandle_t>(0x44444444);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle));

  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(region);
  auto result = alloc.create("TestTimer", 100, pdTRUE, nullptr, nullptr);
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalSwTimerAllocatorFailedAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_sw_timer_allocator<TestMemoryRegion> alloc(fail_region);
  auto result = alloc.create("TestTimer", 100, pdTRUE, nullptr, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalTaskAllocatorCreate) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0x55555555);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle));

  freertos::external_task_allocator<TestMemoryRegion, 1024> alloc(region);
  auto result = alloc.create(nullptr, "TestTask", 1, nullptr);
  EXPECT_EQ(result, handle);
}

TEST_F(ExternalAllocatorTest, ExternalTaskAllocatorFailedTaskAllocation) {
  TestMemoryRegion fail_region{test_allocate_fail, test_deallocate};

  freertos::external_task_allocator<TestMemoryRegion, 1024> alloc(fail_region);
  auto result = alloc.create(nullptr, "TestTask", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, ExternalTaskAllocatorFailedStackAllocation) {
  TestMemoryRegion partial_fail_region{test_allocate_fail, test_deallocate};

  freertos::external_task_allocator<TestMemoryRegion, 1024> alloc(
      partial_fail_region);
  auto result = alloc.create(nullptr, "TestTask", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalAllocatorTest, EaNamespaceAliasesExist) {
  static_assert(std::is_same_v<
                freertos::ea::binary_semaphore<TestMemoryRegion>,
                freertos::binary_semaphore<
                    freertos::external_semaphore_allocator<TestMemoryRegion>>>);

  static_assert(std::is_same_v<
                freertos::ea::counting_semaphore<TestMemoryRegion>,
                freertos::counting_semaphore<
                    freertos::external_semaphore_allocator<TestMemoryRegion>>>);

  static_assert(
      std::is_same_v<freertos::ea::mutex<TestMemoryRegion>,
                     freertos::mutex<freertos::external_semaphore_allocator<
                         TestMemoryRegion>>>);

  static_assert(std::is_same_v<
                freertos::ea::recursive_mutex<TestMemoryRegion>,
                freertos::recursive_mutex<
                    freertos::external_semaphore_allocator<TestMemoryRegion>>>);

  static_assert(
      std::is_same_v<
          freertos::ea::event_group<TestMemoryRegion>,
          freertos::event_group<
              freertos::external_event_group_allocator<TestMemoryRegion>>>);

  static_assert(
      std::is_same_v<freertos::ea::queue<TestMemoryRegion, 10, int>,
                     freertos::queue<10, int,
                                     freertos::external_queue_allocator<
                                         TestMemoryRegion, 10, int>>>);

  static_assert(
      std::is_same_v<freertos::ea::stream_buffer<TestMemoryRegion, 64>,
                     freertos::stream_buffer<
                         64, freertos::external_stream_buffer_allocator<
                                 TestMemoryRegion, 64>>>);

  static_assert(
      std::is_same_v<freertos::ea::message_buffer<TestMemoryRegion, 128>,
                     freertos::message_buffer<
                         128, freertos::external_message_buffer_allocator<
                                  TestMemoryRegion, 128>>>);

  static_assert(
      std::is_same_v<freertos::ea::timer<TestMemoryRegion>,
                     freertos::timer<freertos::external_sw_timer_allocator<
                         TestMemoryRegion>>>);

  static_assert(std::is_same_v<freertos::ea::task<TestMemoryRegion, 512>,
                               freertos::task<freertos::external_task_allocator<
                                   TestMemoryRegion, 512>>>);

  static_assert(
      std::is_same_v<freertos::ea::periodic_task<TestMemoryRegion, 512>,
                     freertos::periodic_task<freertos::external_task_allocator<
                         TestMemoryRegion, 512>>>);
}
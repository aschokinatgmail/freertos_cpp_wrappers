/**
 * @file test_freertos_move_semantics.cpp
 * @brief Comprehensive unit tests for move semantics across all FreeRTOS C++
 * wrapper classes
 * @version 1.0
 *
 * This file tests move constructors, move assignment operators, and swap
 * operations for all classes that support them:
 * - binary_semaphore, counting_semaphore, mutex, recursive_mutex
 * - queue
 * - event_group
 * - stream_buffer, message_buffer
 * - timer (sw_timer)
 * - task, periodic_task
 *
 * For each class, the following are verified:
 * 1. Move constructor transfers ownership (original handle becomes
 * null/invalid)
 * 2. Move assignment transfers ownership
 * 3. swap() exchanges state between two objects
 * 4. Moved-from object is in a valid-but-unspecified state
 * 5. No copy operations are available (static_assert or compile-time check)
 */

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "FreeRTOS.h"
#include "freertos_event_group.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_queue.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_task.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrEq;
using ::testing::StrictMock;

// =============================================================================
// Static Asserts: Copy Operations Are Deleted
// =============================================================================

static_assert(
    !std::is_copy_constructible_v<
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>>,
    "binary_semaphore must not be copy constructible");
static_assert(
    !std::is_copy_assignable_v<
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>>,
    "binary_semaphore must not be copy assignable");

static_assert(
    !std::is_copy_constructible_v<
        freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>>,
    "counting_semaphore must not be copy constructible");
static_assert(
    !std::is_copy_assignable_v<
        freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>>,
    "counting_semaphore must not be copy assignable");

static_assert(!std::is_copy_constructible_v<
                  freertos::mutex<freertos::dynamic_semaphore_allocator>>,
              "mutex must not be copy constructible");
static_assert(!std::is_copy_assignable_v<
                  freertos::mutex<freertos::dynamic_semaphore_allocator>>,
              "mutex must not be copy assignable");

static_assert(
    !std::is_copy_constructible_v<
        freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>,
    "recursive_mutex must not be copy constructible");
static_assert(
    !std::is_copy_assignable_v<
        freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>,
    "recursive_mutex must not be copy assignable");

static_assert(!std::is_copy_constructible_v<freertos::queue<
                  5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>>,
              "queue must not be copy constructible");
static_assert(!std::is_copy_assignable_v<freertos::queue<
                  5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>>,
              "queue must not be copy assignable");

static_assert(
    !std::is_copy_constructible_v<
        freertos::event_group<freertos::dynamic_event_group_allocator>>,
    "event_group must not be copy constructible");
static_assert(
    !std::is_copy_assignable_v<
        freertos::event_group<freertos::dynamic_event_group_allocator>>,
    "event_group must not be copy assignable");

static_assert(!std::is_copy_constructible_v<freertos::stream_buffer<
                  64, freertos::dynamic_stream_buffer_allocator<64>>>,
              "stream_buffer must not be copy constructible");
static_assert(!std::is_copy_assignable_v<freertos::stream_buffer<
                  64, freertos::dynamic_stream_buffer_allocator<64>>>,
              "stream_buffer must not be copy assignable");

static_assert(!std::is_copy_constructible_v<freertos::message_buffer<
                  256, freertos::dynamic_message_buffer_allocator<256>>>,
              "message_buffer must not be copy constructible");
static_assert(!std::is_copy_assignable_v<freertos::message_buffer<
                  256, freertos::dynamic_message_buffer_allocator<256>>>,
              "message_buffer must not be copy assignable");

static_assert(!std::is_copy_constructible_v<
                  freertos::timer<freertos::dynamic_sw_timer_allocator>>,
              "timer must not be copy constructible");
static_assert(!std::is_copy_assignable_v<
                  freertos::timer<freertos::dynamic_sw_timer_allocator>>,
              "timer must not be copy assignable");

static_assert(!std::is_copy_constructible_v<
                  freertos::task<freertos::dynamic_task_allocator<1024>>>,
              "task must not be copy constructible");
static_assert(!std::is_copy_assignable_v<
                  freertos::task<freertos::dynamic_task_allocator<1024>>>,
              "task must not be copy assignable");

// =============================================================================
// Static Asserts: Move Operations Are Enabled
// =============================================================================

static_assert(
    std::is_move_constructible_v<
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>>,
    "binary_semaphore must be move constructible");
static_assert(
    std::is_move_assignable_v<
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>>,
    "binary_semaphore must be move assignable");

static_assert(
    std::is_move_constructible_v<
        freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>>,
    "counting_semaphore must be move constructible");
static_assert(
    std::is_move_assignable_v<
        freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>>,
    "counting_semaphore must be move assignable");

static_assert(std::is_move_constructible_v<
                  freertos::mutex<freertos::dynamic_semaphore_allocator>>,
              "mutex must be move constructible");
static_assert(std::is_move_assignable_v<
                  freertos::mutex<freertos::dynamic_semaphore_allocator>>,
              "mutex must be move assignable");

static_assert(
    std::is_move_constructible_v<
        freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>,
    "recursive_mutex must be move constructible");
static_assert(
    std::is_move_assignable_v<
        freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>,
    "recursive_mutex must be move assignable");

static_assert(std::is_move_constructible_v<freertos::queue<
                  5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>>,
              "queue must be move constructible");
static_assert(std::is_move_assignable_v<freertos::queue<
                  5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>>,
              "queue must be move assignable");

static_assert(
    std::is_move_constructible_v<
        freertos::event_group<freertos::dynamic_event_group_allocator>>,
    "event_group must be move constructible");
static_assert(
    std::is_move_assignable_v<
        freertos::event_group<freertos::dynamic_event_group_allocator>>,
    "event_group must be move assignable");

static_assert(std::is_move_constructible_v<freertos::stream_buffer<
                  64, freertos::dynamic_stream_buffer_allocator<64>>>,
              "stream_buffer must be move constructible");
static_assert(std::is_move_assignable_v<freertos::stream_buffer<
                  64, freertos::dynamic_stream_buffer_allocator<64>>>,
              "stream_buffer must be move assignable");

static_assert(std::is_move_constructible_v<freertos::message_buffer<
                  256, freertos::dynamic_message_buffer_allocator<256>>>,
              "message_buffer must be move constructible");
static_assert(std::is_move_assignable_v<freertos::message_buffer<
                  256, freertos::dynamic_message_buffer_allocator<256>>>,
              "message_buffer must be move assignable");

static_assert(std::is_move_constructible_v<
                  freertos::timer<freertos::static_sw_timer_allocator>>,
              "timer must be move constructible");
static_assert(std::is_move_assignable_v<
                  freertos::timer<freertos::static_sw_timer_allocator>>,
              "timer must be move assignable");

static_assert(std::is_move_constructible_v<
                  freertos::task<freertos::static_task_allocator<1024>>>,
              "task must be move constructible");
static_assert(std::is_move_assignable_v<
                  freertos::task<freertos::static_task_allocator<1024>>>,
              "task must be move assignable");

// =============================================================================
// Test Fixture
// =============================================================================

class FreeRTOSMoveSemanticsTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
};

// Helper: empty task functions
namespace {
void empty_task_routine() {}
void empty_on_start() {}
void empty_on_stop() {}
void empty_periodic_routine() {}
} // namespace

// =============================================================================
// Binary Semaphore Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2(
      std::move(sem1));

  EXPECT_CALL(*mock, xSemaphoreGive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(sem2.give());
}

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreMoveAssignment) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2;
  sem1 = std::move(sem2);
}

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreSwap) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2;
  sem1.swap(sem2);
}

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreMovedFromIsValid) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2(
      std::move(sem1));

  EXPECT_CALL(*mock, xSemaphoreGive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(sem2.give());
}

// =============================================================================
// Counting Semaphore Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, CountingSemaphoreMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xBBBB);
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem1(5);
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem2(
      std::move(sem1));

  EXPECT_CALL(*mock, xSemaphoreGive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(sem2.give());
}

TEST_F(FreeRTOSMoveSemanticsTest, CountingSemaphoreMoveAssignment) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem1(5);
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem2(5);
  sem1 = std::move(sem2);
}

TEST_F(FreeRTOSMoveSemanticsTest, CountingSemaphoreSwap) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem1(5);
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem2(5);
  sem1.swap(sem2);
}

// =============================================================================
// Mutex Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, MutexMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xCCCC);
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx1;
  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx2(std::move(mtx1));

  EXPECT_CALL(*mock, xSemaphoreGive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(mtx2.unlock());
}

TEST_F(FreeRTOSMoveSemanticsTest, MutexMoveAssignment) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx1;
  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx2;
  mtx1 = std::move(mtx2);
}

TEST_F(FreeRTOSMoveSemanticsTest, MutexSwap) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx1;
  freertos::mutex<freertos::dynamic_semaphore_allocator> mtx2;
  mtx1.swap(mtx2);
}

// =============================================================================
// Recursive Mutex Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, RecursiveMutexMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xDDDD);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx1;
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx2(
      std::move(rmtx1));

  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(rmtx2.lock(), pdTRUE);
  EXPECT_EQ(rmtx2.unlock(), pdTRUE);
}

TEST_F(FreeRTOSMoveSemanticsTest, RecursiveMutexMoveAssignment) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx1;
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx2;
  rmtx1 = std::move(rmtx2);
}

TEST_F(FreeRTOSMoveSemanticsTest, RecursiveMutexSwap) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx1;
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rmtx2;
  rmtx1.swap(rmtx2);
}

// =============================================================================
// Queue Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, QueueMoveConstructor) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, pcQueueGetName(handle)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xQueueSend(handle, NotNull(), portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle)).Times(1);

  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q1;
  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q2(std::move(q1));

  uint32_t val = 42;
  EXPECT_EQ(q2.send(val, portMAX_DELAY), pdPASS);
}

TEST_F(FreeRTOSMoveSemanticsTest, QueueMoveAssignment) {
  QueueHandle_t handle1 = reinterpret_cast<QueueHandle_t>(0x1111);
  QueueHandle_t handle2 = reinterpret_cast<QueueHandle_t>(0x2222);

  EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, pcQueueGetName(handle1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, pcQueueGetName(handle2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vQueueDelete(handle2)).Times(1);

  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q1;
  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q2;
  q1 = std::move(q2);
}

TEST_F(FreeRTOSMoveSemanticsTest, QueueSwap) {
  QueueHandle_t handle1 = reinterpret_cast<QueueHandle_t>(0x1111);
  QueueHandle_t handle2 = reinterpret_cast<QueueHandle_t>(0x2222);

  EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, pcQueueGetName(handle1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, pcQueueGetName(handle2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vQueueDelete(handle2)).Times(1);

  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q1;
  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q2;
  q1.swap(q2);
}

// =============================================================================
// Event Group Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, EventGroupMoveConstructor) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2(
      std::move(eg1));
}

TEST_F(FreeRTOSMoveSemanticsTest, EventGroupMoveAssignment) {
  EventGroupHandle_t handle1 = reinterpret_cast<EventGroupHandle_t>(0x1111);
  EventGroupHandle_t handle2 = reinterpret_cast<EventGroupHandle_t>(0x2222);

  EXPECT_CALL(*mock, xEventGroupCreate())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vEventGroupDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vEventGroupDelete(handle2)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2;
  eg1 = std::move(eg2);
}

TEST_F(FreeRTOSMoveSemanticsTest, EventGroupSwap) {
  EventGroupHandle_t handle1 = reinterpret_cast<EventGroupHandle_t>(0x1111);
  EventGroupHandle_t handle2 = reinterpret_cast<EventGroupHandle_t>(0x2222);

  EXPECT_CALL(*mock, xEventGroupCreate())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vEventGroupDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vEventGroupDelete(handle2)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2;
  eg1.swap(eg2);
}

// =============================================================================
// Stream Buffer Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, StreamBufferMoveConstructor) {
  StreamBufferHandle_t handle = reinterpret_cast<StreamBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2(std::move(sb1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StreamBufferMoveAssignment) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2)).Times(1);

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2;
  sb1 = std::move(sb2);
}

TEST_F(FreeRTOSMoveSemanticsTest, StreamBufferSwap) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2)).Times(1);

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2;
  sb1.swap(sb2);
}

// =============================================================================
// Message Buffer Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, MessageBufferMoveConstructor) {
  MessageBufferHandle_t handle =
      reinterpret_cast<MessageBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xMessageBufferCreate(512)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle)).Times(1);

  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb1;
  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb2(std::move(mb1));
}

TEST_F(FreeRTOSMoveSemanticsTest, MessageBufferMoveAssignment) {
  MessageBufferHandle_t handle1 =
      reinterpret_cast<MessageBufferHandle_t>(0x1111);
  MessageBufferHandle_t handle2 =
      reinterpret_cast<MessageBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xMessageBufferCreate(512))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vMessageBufferDelete(handle2)).Times(1);

  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb1;
  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb2;
  mb1 = std::move(mb2);
}

TEST_F(FreeRTOSMoveSemanticsTest, MessageBufferSwap) {
  MessageBufferHandle_t handle1 =
      reinterpret_cast<MessageBufferHandle_t>(0x1111);
  MessageBufferHandle_t handle2 =
      reinterpret_cast<MessageBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xMessageBufferCreate(512))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vMessageBufferDelete(handle2)).Times(1);

  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb1;
  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb2;
  mb1.swap(mb2);
}

// =============================================================================
// Timer (sw_timer) Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, TimerMoveConstructor) {
  TimerHandle_t handle = reinterpret_cast<TimerHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vTimerSetTimerID(handle, _));
  EXPECT_CALL(*mock, xTimerDelete(handle, portMAX_DELAY)).Times(1);

  auto callback = []() {};
  freertos::sa::timer t1("Timer1", 1000, pdTRUE, callback);
  freertos::sa::timer t2(std::move(t1));

  EXPECT_CALL(*mock, xTimerStart(handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_EQ(t2.start(), pdPASS);
}

TEST_F(FreeRTOSMoveSemanticsTest, TimerSwap) {
  TimerHandle_t handle1 = reinterpret_cast<TimerHandle_t>(0x1111);
  TimerHandle_t handle2 = reinterpret_cast<TimerHandle_t>(0x2222);

  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vTimerSetTimerID(handle1, _)).Times(1);
  EXPECT_CALL(*mock, vTimerSetTimerID(handle2, _)).Times(1);
  EXPECT_CALL(*mock, xTimerDelete(handle1, portMAX_DELAY)).Times(1);
  EXPECT_CALL(*mock, xTimerDelete(handle2, portMAX_DELAY)).Times(1);

  auto cb1 = []() {};
  auto cb2 = []() {};
  freertos::sa::timer t1("Timer1", 1000, pdTRUE, cb1);
  freertos::sa::timer t2("Timer2", 2000, pdFALSE, cb2);
  t1.swap(t2);
}

// =============================================================================
// Task Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, TaskMoveConstructor) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vTaskDelete(handle)).Times(1);

  freertos::sa::task<1024> t1("MoveTest", 2, empty_task_routine);
  freertos::sa::task<1024> t2(std::move(t1));

  EXPECT_EQ(t2.handle(), handle);
  EXPECT_EQ(t1.handle(), nullptr);
}

TEST_F(FreeRTOSMoveSemanticsTest, TaskMoveAssignment) {
  TaskHandle_t handle1 = reinterpret_cast<TaskHandle_t>(0x1111);
  TaskHandle_t handle2 = reinterpret_cast<TaskHandle_t>(0x2222);

  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vTaskDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vTaskDelete(handle2)).Times(1);

  freertos::sa::task<1024> t1("Task1", 2, empty_task_routine);
  freertos::sa::task<1024> t2("Task2", 2, empty_task_routine);
  t1 = std::move(t2);
}

TEST_F(FreeRTOSMoveSemanticsTest, TaskSwap) {
  TaskHandle_t handle1 = reinterpret_cast<TaskHandle_t>(0x1111);
  TaskHandle_t handle2 = reinterpret_cast<TaskHandle_t>(0x2222);

  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vTaskDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vTaskDelete(handle2)).Times(1);

  freertos::sa::task<1024> t1("Task1", 2, empty_task_routine);
  freertos::sa::task<1024> t2("Task2", 2, empty_task_routine);
  t1.swap(t2);
}

TEST_F(FreeRTOSMoveSemanticsTest, TaskMovedFromIsInvalid) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vTaskDelete(handle)).Times(1);

  freertos::sa::task<1024> t1("MoveInvalidationTest", 2, empty_task_routine);
  freertos::sa::task<1024> t2(std::move(t1));

  EXPECT_NE(t2.handle(), nullptr);
  EXPECT_EQ(t1.handle(), nullptr);
}

// =============================================================================
// Periodic Task Move Semantics
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, PeriodicTaskMoveConstructor) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(handle)).Times(1);

  freertos::sa::periodic_task<1024> pt1("PeriodicMoveTest", 2, empty_on_start,
                                        empty_on_stop, empty_periodic_routine,
                                        std::chrono::milliseconds(100));
  freertos::sa::periodic_task<1024> pt2(std::move(pt1));

  EXPECT_EQ(pt2.handle(), handle);
  EXPECT_EQ(pt1.handle(), nullptr);
}

TEST_F(FreeRTOSMoveSemanticsTest, PeriodicTaskSwap) {
  TaskHandle_t handle1 = reinterpret_cast<TaskHandle_t>(0x1111);
  TaskHandle_t handle2 = reinterpret_cast<TaskHandle_t>(0x2222);

  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("PT1"), _, _, _, _, _))
      .WillOnce(Return(handle1));
  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("PT2"), _, _, _, _, _))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, xTaskAbortDelay(handle1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskAbortDelay(handle2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vTaskDelete(handle2)).Times(1);

  freertos::sa::periodic_task<1024> pt1("PT1", 2, empty_on_start, empty_on_stop,
                                        empty_periodic_routine,
                                        std::chrono::milliseconds(100));
  freertos::sa::periodic_task<1024> pt2("PT2", 2, empty_on_start, empty_on_stop,
                                        empty_periodic_routine,
                                        std::chrono::milliseconds(200));
  pt1.swap(pt2);
}

// =============================================================================
// Static Allocator Types Move Semantics
// =============================================================================

#if configSUPPORT_STATIC_ALLOCATION

TEST_F(FreeRTOSMoveSemanticsTest, StaticBinarySemaphoreMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::sa::binary_semaphore sem1;
  freertos::sa::binary_semaphore sem2(std::move(sem1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StaticMutexMoveConstructor) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xCCCC);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::sa::mutex mtx1;
  freertos::sa::mutex mtx2(std::move(mtx1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StaticEventGroupMoveConstructor) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle)).Times(1);

  freertos::sa::event_group eg1;
  freertos::sa::event_group eg2(std::move(eg1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StaticQueueMoveConstructor) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0xEEEE);
  EXPECT_CALL(*mock,
              xQueueCreateStatic(4, sizeof(uint32_t), NotNull(), NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, pcQueueGetName(handle)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(handle)).Times(1);

  freertos::sa::queue<4, uint32_t> q1;
  freertos::sa::queue<4, uint32_t> q2(std::move(q1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StaticStreamBufferMoveConstructor) {
  StreamBufferHandle_t handle = reinterpret_cast<StreamBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(100, 1, NotNull(), NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::sa::stream_buffer<100> sb1;
  freertos::sa::stream_buffer<100> sb2(std::move(sb1));
}

TEST_F(FreeRTOSMoveSemanticsTest, StaticMessageBufferMoveConstructor) {
  MessageBufferHandle_t handle =
      reinterpret_cast<MessageBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle)).Times(1);

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2(std::move(mb1));
}

#endif // configSUPPORT_STATIC_ALLOCATION

// =============================================================================
// Move Semantics with Self-Assignment
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreSelfMoveAssignment) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  sem = std::move(sem);
}

TEST_F(FreeRTOSMoveSemanticsTest, EventGroupSelfMoveAssignment) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg;
  eg = std::move(eg);
}

// =============================================================================
// Swap via ADL (free function swap)
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, BinarySemaphoreADLSwap) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x1111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x2222);

  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2;
  using std::swap;
  swap(sem1, sem2);
}

TEST_F(FreeRTOSMoveSemanticsTest, EventGroupADLSwap) {
  EventGroupHandle_t handle1 = reinterpret_cast<EventGroupHandle_t>(0x1111);
  EventGroupHandle_t handle2 = reinterpret_cast<EventGroupHandle_t>(0x2222);

  EXPECT_CALL(*mock, xEventGroupCreate())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vEventGroupDelete(handle1)).Times(1);
  EXPECT_CALL(*mock, vEventGroupDelete(handle2)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2;
  using std::swap;
  swap(eg1, eg2);
}

// =============================================================================
// Move semantics verify destruction of moved-from objects is safe
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, MovedFromBinarySemaphoreDestructorSafe) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2(
      std::move(sem1));
  // sem1 destructor called here - should be safe since handle is nullptr
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedFromEventGroupDestructorSafe) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2(
      std::move(eg1));
  // eg1 destructor called here - should be safe since handle is nullptr
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedFromQueueDestructorSafe) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, pcQueueGetName(handle)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(handle)).Times(1);

  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q1;
  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q2(std::move(q1));
  // q1 destructor called - should be safe since handle is nullptr
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedFromStreamBufferDestructorSafe) {
  StreamBufferHandle_t handle = reinterpret_cast<StreamBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle)).Times(1);

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2(std::move(sb1));
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedFromMessageBufferDestructorSafe) {
  MessageBufferHandle_t handle =
      reinterpret_cast<MessageBufferHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xMessageBufferCreate(512)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle)).Times(1);

  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb1;
  freertos::message_buffer<512, freertos::dynamic_message_buffer_allocator<512>>
      mb2(std::move(mb1));
}

// =============================================================================
// Move semantics preserve operation capability on moved-to object
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, MovedToBinarySemaphoreOperational) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xAAAA);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xSemaphoreGive(handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle)).Times(1);

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem1;
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem2(
      std::move(sem1));

  auto result = sem2.give();
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedToEventGroupOperational) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(handle));
  EXPECT_CALL(*mock, xEventGroupSetBits(handle, 0x01)).WillOnce(Return(0x01));
  EXPECT_CALL(*mock, vEventGroupDelete(handle)).Times(1);

  freertos::event_group<freertos::dynamic_event_group_allocator> eg1;
  freertos::event_group<freertos::dynamic_event_group_allocator> eg2(
      std::move(eg1));

  auto result = eg2.set_bits(0x01);
  EXPECT_EQ(result, 0x01);
}

TEST_F(FreeRTOSMoveSemanticsTest, MovedToQueueOperational) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, pcQueueGetName(handle)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xQueueSend(handle, NotNull(), portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(handle)).Times(1);

  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q1;
  freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>>
      q2(std::move(q1));

  uint32_t val = 42;
  auto result = q2.send(val, portMAX_DELAY);
  EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Move semantics handle verification for types that expose handle()
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, TaskMoveConstructorHandleTransfer) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("MoveTest"), _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vTaskDelete(handle)).Times(1);

  freertos::sa::task<1024> original_task("MoveTest", 2, empty_task_routine);
  EXPECT_EQ(original_task.handle(), handle);

  freertos::sa::task<1024> moved_task(std::move(original_task));
  EXPECT_EQ(moved_task.handle(), handle);
  EXPECT_EQ(original_task.handle(), nullptr);
}

TEST_F(FreeRTOSMoveSemanticsTest, PeriodicTaskMoveConstructorHandleTransfer) {
  TaskHandle_t handle = reinterpret_cast<TaskHandle_t>(0xEEEE);
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(handle)).Times(1);

  freertos::sa::periodic_task<1024> original_task(
      "MoveTest", 2, empty_on_start, empty_on_stop, empty_periodic_routine,
      std::chrono::milliseconds(100));
  EXPECT_EQ(original_task.handle(), handle);

  freertos::sa::periodic_task<1024> moved_task(std::move(original_task));
  EXPECT_EQ(moved_task.handle(), handle);
  EXPECT_EQ(original_task.handle(), nullptr);
}

// =============================================================================
// Timer move assignment (complex scenario)
// =============================================================================

TEST_F(FreeRTOSMoveSemanticsTest, TimerMoveAssignment) {
  TimerHandle_t src_handle = reinterpret_cast<TimerHandle_t>(0x1111);
  TimerHandle_t dest_handle = reinterpret_cast<TimerHandle_t>(0x2222);

  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(src_handle))
      .WillOnce(Return(dest_handle));

  auto cb1 = []() {};
  auto cb2 = []() {};
  freertos::sa::timer src_timer("SrcTimer", 1000, pdTRUE, cb1);
  freertos::sa::timer dest_timer("DestTimer", 2000, pdFALSE, cb2);

  EXPECT_CALL(*mock, xTimerDelete(dest_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(src_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(src_handle)).WillOnce(Return("SrcTimer"));
  EXPECT_CALL(*mock, xTimerGetPeriod(src_handle)).WillOnce(Return(1000));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(src_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(src_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  TimerHandle_t new_handle = reinterpret_cast<TimerHandle_t>(0x3333);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(new_handle));
  EXPECT_CALL(*mock, xTimerDelete(new_handle, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  dest_timer = std::move(src_timer);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
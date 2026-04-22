#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_heap.hpp"
#include "freertos_mocks.hpp"

using ::testing::Eq;
using ::testing::Return;

class HeapTest : public ::testing::Test {
protected:
    std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;

    void SetUp() override {
        mock = FreeRTOSMockInstance::getInstance();
    }

    void TearDown() override {
        FreeRTOSMockInstance::resetInstance();
    }
};

TEST_F(HeapTest, AllocateCallsPvPortMalloc) {
    void *expected = reinterpret_cast<void *>(0x1234);
    EXPECT_CALL(*mock, pvPortMalloc(64))
        .WillOnce(Return(expected));
    auto *ptr = freertos::heap::allocate(64);
    EXPECT_EQ(ptr, expected);
}

TEST_F(HeapTest, AllocateZeroSize) {
    EXPECT_CALL(*mock, pvPortMalloc(0))
        .WillOnce(Return(nullptr));
    auto *ptr = freertos::heap::allocate(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(HeapTest, AllocateAlignedCallsPvPortMalloc) {
    void *expected = reinterpret_cast<void *>(0x5678);
    EXPECT_CALL(*mock, pvPortMalloc(128))
        .WillOnce(Return(expected));
    auto *ptr = freertos::heap::allocate_aligned(8, 128);
    EXPECT_EQ(ptr, expected);
}

TEST_F(HeapTest, DeallocateCallsVPortFree) {
    void *ptr = reinterpret_cast<void *>(0x1234);
    EXPECT_CALL(*mock, vPortFree(ptr));
    freertos::heap::deallocate(ptr);
}

TEST_F(HeapTest, DeallocateNullIsSafe) {
    EXPECT_CALL(*mock, vPortFree(nullptr));
    freertos::heap::deallocate(nullptr);
}

TEST_F(HeapTest, CallocCallsPvPortCalloc) {
    void *expected = reinterpret_cast<void *>(0xABCD);
    EXPECT_CALL(*mock, pvPortCalloc(4, 32))
        .WillOnce(Return(expected));
    auto *ptr = freertos::heap::calloc(4, 32);
    EXPECT_EQ(ptr, expected);
}

TEST_F(HeapTest, EnableRedirectSetsFlag) {
    freertos::heap::enable_redirect();
    EXPECT_TRUE(freertos::heap::is_redirect_enabled());
}

TEST_F(HeapTest, DisableRedirectClearsFlag) {
    freertos::heap::disable_redirect();
    EXPECT_FALSE(freertos::heap::is_redirect_enabled());
}

TEST_F(HeapTest, ToggleRedirectFlag) {
    freertos::heap::disable_redirect();
    EXPECT_FALSE(freertos::heap::is_redirect_enabled());
    freertos::heap::enable_redirect();
    EXPECT_TRUE(freertos::heap::is_redirect_enabled());
}

TEST_F(HeapTest, RedirectFlagDefaultTrue) {
    EXPECT_TRUE(freertos::heap::is_redirect_enabled());
}

#if configSUPPORT_STATIC_ALLOCATION

class StaticMemoryTest : public ::testing::Test {
protected:
    std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;

    void SetUp() override {
        mock = FreeRTOSMockInstance::getInstance();
    }

    void TearDown() override {
        FreeRTOSMockInstance::resetInstance();
    }
};

TEST_F(StaticMemoryTest, DefaultIdleTaskStackSizeMatchesConfig) {
    EXPECT_EQ(freertos::static_memory::default_idle_task_stack_size,
              static_cast<UBaseType_t>(configMINIMAL_STACK_SIZE));
}

TEST_F(StaticMemoryTest, DefaultTimerTaskStackSizeMatchesConfig) {
    EXPECT_EQ(freertos::static_memory::default_timer_task_stack_size,
              configTIMER_TASK_STACK_DEPTH);
}

TEST_F(StaticMemoryTest, IdleTaskMemoryReturnsNonNullPointers) {
    StaticTask_t *tcb = nullptr;
    StackType_t *stack = nullptr;
    uint32_t stack_size = 0;

    freertos::static_memory::vApplicationGetIdleTaskMemory(
        &tcb, &stack, &stack_size);

    EXPECT_NE(tcb, nullptr);
    EXPECT_NE(stack, nullptr);
    EXPECT_EQ(stack_size, static_cast<uint32_t>(configMINIMAL_STACK_SIZE));
}

TEST_F(StaticMemoryTest, TimerTaskMemoryReturnsNonNullPointers) {
    StaticTask_t *tcb = nullptr;
    StackType_t *stack = nullptr;
    uint32_t stack_size = 0;

    freertos::static_memory::vApplicationGetTimerTaskMemory(
        &tcb, &stack, &stack_size);

    EXPECT_NE(tcb, nullptr);
    EXPECT_NE(stack, nullptr);
    EXPECT_EQ(stack_size, static_cast<uint32_t>(configTIMER_TASK_STACK_DEPTH));
}

TEST_F(StaticMemoryTest, IdleTaskMemoryReturnsConsistentPointers) {
    StaticTask_t *tcb1 = nullptr;
    StackType_t *stack1 = nullptr;
    uint32_t size1 = 0;

    freertos::static_memory::vApplicationGetIdleTaskMemory(
        &tcb1, &stack1, &size1);

    StaticTask_t *tcb2 = nullptr;
    StackType_t *stack2 = nullptr;
    uint32_t size2 = 0;

    freertos::static_memory::vApplicationGetIdleTaskMemory(
        &tcb2, &stack2, &size2);

    EXPECT_EQ(tcb1, tcb2);
    EXPECT_EQ(stack1, stack2);
    EXPECT_EQ(size1, size2);
}

TEST_F(StaticMemoryTest, TimerTaskMemoryReturnsConsistentPointers) {
    StaticTask_t *tcb1 = nullptr;
    StackType_t *stack1 = nullptr;
    uint32_t size1 = 0;

    freertos::static_memory::vApplicationGetTimerTaskMemory(
        &tcb1, &stack1, &size1);

    StaticTask_t *tcb2 = nullptr;
    StackType_t *stack2 = nullptr;
    uint32_t size2 = 0;

    freertos::static_memory::vApplicationGetTimerTaskMemory(
        &tcb2, &stack2, &size2);

    EXPECT_EQ(tcb1, tcb2);
    EXPECT_EQ(stack1, stack2);
    EXPECT_EQ(size1, size2);
}

TEST_F(StaticMemoryTest, IdleAndTimerTaskMemoryAreDistinct) {
    StaticTask_t *idle_tcb = nullptr;
    StackType_t *idle_stack = nullptr;
    uint32_t idle_size = 0;

    freertos::static_memory::vApplicationGetIdleTaskMemory(
        &idle_tcb, &idle_stack, &idle_size);

    StaticTask_t *timer_tcb = nullptr;
    StackType_t *timer_stack = nullptr;
    uint32_t timer_size = 0;

    freertos::static_memory::vApplicationGetTimerTaskMemory(
        &timer_tcb, &timer_stack, &timer_size);

    EXPECT_NE(idle_tcb, timer_tcb);
    EXPECT_NE(idle_stack, timer_stack);
}

#endif // configSUPPORT_STATIC_ALLOCATION
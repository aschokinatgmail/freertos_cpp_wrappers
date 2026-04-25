#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <new>

#include "FreeRTOS.h"
#include "freertos_expected.hpp"
#include "freertos_pend_call.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::StrictMock;

class PendCallTest : public ::testing::Test {
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

static void test_callback(void *arg1, uint32_t arg2) {
  (void)arg1;
  (void)arg2;
}

TEST_F(PendCallTest, PendCallRawFunctionPointerSuccess) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 42, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call(test_callback, nullptr, 42);
  EXPECT_TRUE(result);
}

TEST_F(PendCallTest, PendCallRawFunctionPointerFailure) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));

  auto result = freertos::pend_call(test_callback, nullptr, 0);
  EXPECT_FALSE(result);
}

TEST_F(PendCallTest, PendCallRawFunctionPointerWithTimeout) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, 100))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call(test_callback, nullptr, 0, 100);
  EXPECT_TRUE(result);
}

TEST_F(PendCallTest, PendCallExSuccess) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call_ex(test_callback, nullptr, 0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(PendCallTest, PendCallExFailure) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));

  auto result = freertos::pend_call_ex(test_callback, nullptr, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(PendCallTest, PendCallExWithTimeout) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, 500))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call_ex(test_callback, nullptr, 0, 500);
  EXPECT_TRUE(result.has_value());
}

TEST_F(PendCallTest, PendCallLambdaSuccess) {
  bool called = false;
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call([&called]() { called = true; });
  EXPECT_TRUE(result);
}

TEST_F(PendCallTest, PendCallLambdaFailureDeletesCallable) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));

  auto result = freertos::pend_call([]() {});
  EXPECT_FALSE(result);
}

TEST_F(PendCallTest, PendCallFunctorSuccess) {
  struct Functor {
    int &counter;
    void operator()() { counter++; }
  };
  int counter = 0;

  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(Return(pdPASS));

  auto result = freertos::pend_call(Functor{counter});
  EXPECT_TRUE(result);
}

namespace {
struct FailOnNewCallable {
  int value;
  static bool allocation_should_fail;

  void operator()() {}

  static void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
    if (allocation_should_fail) {
      return nullptr;
    }
    return ::operator new(size, std::nothrow);
  }

  static void operator delete(void *ptr) noexcept { ::operator delete(ptr); }
};

bool FailOnNewCallable::allocation_should_fail = false;
} // namespace

TEST_F(PendCallTest, PendCallLambdaOOMReturnsFalse) {
  FailOnNewCallable::allocation_should_fail = true;
  auto result = freertos::pend_call(FailOnNewCallable{42});
  FailOnNewCallable::allocation_should_fail = false;
  EXPECT_FALSE(result);
}

class PendCallExTest : public ::testing::Test {
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

TEST_F(PendCallExTest, PendCallExRawFunctionQueueFullReturnsTimeout) {
  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, nullptr, 0, portMAX_DELAY))
      .WillOnce(Return(errQUEUE_FULL));

  auto result = freertos::pend_call_ex(test_callback, nullptr, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(PendCallTest, PendCallCallableAdapterInvokesTheCallable) {
  bool called = false;
  PendedFunction_t captured_adapter = nullptr;
  void *captured_ptr = nullptr;

  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(DoAll(SaveArg<0>(&captured_adapter),
                      SaveArg<1>(&captured_ptr), Return(pdPASS)));

  auto result = freertos::pend_call([&called]() { called = true; });
  EXPECT_TRUE(result);

  ASSERT_NE(captured_adapter, nullptr);
  ASSERT_NE(captured_ptr, nullptr);
  EXPECT_FALSE(called);

  captured_adapter(captured_ptr, 0);

  EXPECT_TRUE(called);
}

TEST_F(PendCallTest, PendCallCallableAdapterDeletesCallableAfterInvocation) {
  static int alive_count;
  alive_count = 0;

  struct CountingCallable {
    int *target;
    int *alive_count_ptr;
    CountingCallable(int *t, int *ac) : target(t), alive_count_ptr(ac) { (*alive_count_ptr)++; }
    CountingCallable(const CountingCallable &other) : target(other.target), alive_count_ptr(other.alive_count_ptr) {
      (*alive_count_ptr)++;
    }
    ~CountingCallable() { (*alive_count_ptr)--; }
    void operator()() { (*target)++; }
  };

  int value = 0;
  PendedFunction_t captured_adapter = nullptr;
  void *captured_ptr = nullptr;

  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(DoAll(SaveArg<0>(&captured_adapter),
                      SaveArg<1>(&captured_ptr), Return(pdPASS)));

  auto result = freertos::pend_call(CountingCallable(&value, &alive_count));
  EXPECT_TRUE(result);
  ASSERT_NE(captured_adapter, nullptr);
  ASSERT_NE(captured_ptr, nullptr);

  captured_adapter(captured_ptr, 0);

  EXPECT_EQ(value, 1);
  EXPECT_EQ(alive_count, 0);
}

TEST_F(PendCallTest, PendCallLambdaOOMDoesNotCallPendFunction) {
  FailOnNewCallable::allocation_should_fail = true;
  EXPECT_CALL(*mock, xTimerPendFunctionCall(_, _, _, _)).Times(0);
  auto result = freertos::pend_call(FailOnNewCallable{42});
  FailOnNewCallable::allocation_should_fail = false;
  EXPECT_FALSE(result);
}

TEST_F(PendCallTest, PendCallLambdaFailurePathDeletesCallable) {
  static int alive_count;
  alive_count = 0;

  struct TrackedCallable {
    bool *called;
    int *alive_count_ptr;
    TrackedCallable(bool *c, int *ac) : called(c), alive_count_ptr(ac) { (*alive_count_ptr)++; }
    TrackedCallable(const TrackedCallable &other) : called(other.called), alive_count_ptr(other.alive_count_ptr) {
      (*alive_count_ptr)++;
    }
    ~TrackedCallable() { (*alive_count_ptr)--; }
    void operator()() { *called = true; }
  };

  EXPECT_CALL(*mock,
              xTimerPendFunctionCall(_, _, 0, portMAX_DELAY))
      .WillOnce(Return(pdFAIL));

  bool called = false;
  auto result = freertos::pend_call(TrackedCallable(&called, &alive_count));
  EXPECT_FALSE(result);
  EXPECT_FALSE(called);
  EXPECT_EQ(alive_count, 0);
}
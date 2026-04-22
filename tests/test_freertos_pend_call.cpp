#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_expected.hpp"
#include "freertos_pend_call.hpp"

using ::testing::_;
using ::testing::Return;
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
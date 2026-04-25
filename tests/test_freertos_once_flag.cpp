#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_once_flag.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class OnceFlagTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    counter = 0;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  int counter;
};

TEST_F(OnceFlagTest, IsInitialized_DefaultFalse) {
  freertos::once_flag flag;
  EXPECT_FALSE(flag.is_initialized());
}

TEST_F(OnceFlagTest, CallOnce_ExecutesFunction) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  freertos::call_once(flag, [this]() { counter++; });
  EXPECT_EQ(counter, 1);
  EXPECT_TRUE(flag.is_initialized());
}

TEST_F(OnceFlagTest, CallOnce_ExecutesOnlyOnce) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  freertos::call_once(flag, [this]() { counter++; });
  freertos::call_once(flag, [this]() { counter++; });
  freertos::call_once(flag, [this]() { counter++; });
  EXPECT_EQ(counter, 1);
}

TEST_F(OnceFlagTest, CallOnce_WithLambda) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);
  int value = 0;

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  freertos::call_once(flag, [&value]() { value = 42; });
  EXPECT_EQ(value, 42);
  EXPECT_TRUE(flag.is_initialized());
}

TEST_F(OnceFlagTest, CallOnce_WithFunctionPointer) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  static int global_val;
  global_val = 0;
  void (*fp)() = []() { global_val = 99; };
  freertos::call_once(flag, fp);
  EXPECT_EQ(global_val, 99);
}

struct Functor {
  int &ref;
  void operator()() { ref = 77; }
};

TEST_F(OnceFlagTest, CallOnce_WithFunctionObject) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);
  int value = 0;

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  Functor f{value};
  freertos::call_once(flag, f);
  EXPECT_EQ(value, 77);
}

#if configSUPPORT_STATIC_ALLOCATION
TEST_F(OnceFlagTest, StaticAllocation_IsInitializedDefault) {
  freertos::sa::once_flag flag;
  EXPECT_FALSE(flag.is_initialized());
}

TEST_F(OnceFlagTest, StaticAllocation_CallOnce) {
  freertos::sa::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));

  int value = 0;
  freertos::sa::call_once(flag, [&value]() { value = 55; });
  EXPECT_EQ(value, 55);
  EXPECT_TRUE(flag.is_initialized());
}

TEST_F(OnceFlagTest, StaticAllocation_CallOnceOnlyOnce) {
  freertos::sa::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));

  int value = 0;
  freertos::sa::call_once(flag, [&value]() { value++; });
  freertos::sa::call_once(flag, [&value]() { value++; });
  EXPECT_EQ(value, 1);
}
#endif

TEST_F(OnceFlagTest, CallOnce_ThrowsException_Rethrows) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);

  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(sem));
  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  EXPECT_THROW(
      freertos::call_once(flag, []() { throw std::runtime_error("test"); }),
      std::runtime_error);
  EXPECT_FALSE(flag.is_initialized());
}

TEST_F(OnceFlagTest, CallOnce_ThrowsException_RetrySucceeds) {
  freertos::once_flag flag;
  SemaphoreHandle_t sem = reinterpret_cast<SemaphoreHandle_t>(0x1);
  int value = 0;

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
        .WillOnce(Return(sem));
    EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  }

  EXPECT_THROW(
      freertos::call_once(flag, []() { throw std::runtime_error("test"); }),
      std::runtime_error);
  EXPECT_FALSE(flag.is_initialized());

  EXPECT_CALL(*mock, xSemaphoreGive(sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem));

  freertos::call_once(flag, [&value]() { value = 42; });
  EXPECT_EQ(value, 42);
  EXPECT_TRUE(flag.is_initialized());
}

#if configSUPPORT_DYNAMIC_ALLOCATION
TEST_F(OnceFlagTest, DynamicAllocation_NamespaceAlias) {
  freertos::da::once_flag flag;
  EXPECT_FALSE(flag.is_initialized());
}
#endif
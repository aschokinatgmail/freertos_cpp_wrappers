#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_shared_mutex.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

static constexpr UBaseType_t kMaxReaders =
    FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS;

class SharedMutexTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x11111111);
    mock_counting_handle = reinterpret_cast<SemaphoreHandle_t>(0x22222222);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_mutex_handle;
  SemaphoreHandle_t mock_counting_handle;
};

TEST_F(SharedMutexTest, DynamicConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;
}

TEST_F(SharedMutexTest, ExclusiveLockAndUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  sm.lock();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock();
}

TEST_F(SharedMutexTest, SharedLockAndUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  sm.lock_shared();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock_shared();
}

TEST_F(SharedMutexTest, TryLockSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_TRUE(sm.try_lock());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock();
}

TEST_F(SharedMutexTest, TryLockFailsMutexBusy) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_FALSE(sm.try_lock());
}

TEST_F(SharedMutexTest, TryLockFailsReadersActive) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  InSequence s;
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_FALSE(sm.try_lock());
}

TEST_F(SharedMutexTest, TryLockSharedSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_TRUE(sm.try_lock_shared());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock_shared();
}

TEST_F(SharedMutexTest, TryLockSharedFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_FALSE(sm.try_lock_shared());
}

TEST_F(SharedMutexTest, NativeHandle) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;
  EXPECT_EQ(sm.native_handle(), mock_mutex_handle);
}

TEST_F(SharedMutexTest, TryLockForSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskGetTickCount())
      .WillRepeatedly(Return(0));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, _))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_TRUE(sm.try_lock_for(std::chrono::milliseconds(100)));

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock();
}

TEST_F(SharedMutexTest, TryLockSharedForSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_TRUE(sm.try_lock_shared_for(std::chrono::milliseconds(100)));

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock_shared();
}

TEST_F(SharedMutexTest, LockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskGetTickCount())
      .WillRepeatedly(Return(0));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, _))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  auto result = sm.lock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SharedMutexTest, LockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, _))
      .WillOnce(Return(pdFALSE));
  auto result = sm.lock_ex(std::chrono::milliseconds(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SharedMutexTest, TryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  auto result = sm.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SharedMutexTest, TryLockExFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdFALSE));
  auto result = sm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SharedMutexTest, LockSharedExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  auto result = sm.lock_shared_ex();
  EXPECT_TRUE(result.has_value());

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock_shared();
}

TEST_F(SharedMutexTest, LockSharedExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdFALSE));
  auto result = sm.lock_shared_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SharedMutexTest, TryLockSharedExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdTRUE));
  auto result = sm.try_lock_shared_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SharedMutexTest, TryLockSharedExFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, 0))
      .WillOnce(Return(pdFALSE));
  auto result = sm.try_lock_shared_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SharedMutexTest, UnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  sm.lock();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  auto result = sm.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SharedMutexTest, UnlockSharedExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  sm.lock_shared();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  auto result = sm.unlock_shared_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SharedMutexTest, MultipleSharedLocks) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));

  for (UBaseType_t i = 0; i < kMaxReaders; i++) {
    sm.lock_shared();
  }

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));

  for (UBaseType_t i = 0; i < kMaxReaders; i++) {
    sm.unlock_shared();
  }
}

TEST_F(SharedMutexTest, TryLockForFailsDuringReaderDrain) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(kMaxReaders, kMaxReaders))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::da::shared_mutex sm;

  {
    InSequence s;
    EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, _))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, _))
        .WillOnce(Return(pdFALSE));
  }
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTaskGetTickCount())
      .WillRepeatedly(Return(0));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_FALSE(sm.try_lock_for(std::chrono::milliseconds(100)));
}

#if configSUPPORT_STATIC_ALLOCATION

class StaticSharedMutexTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x33333333);
    mock_counting_handle = reinterpret_cast<SemaphoreHandle_t>(0x44444444);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_mutex_handle;
  SemaphoreHandle_t mock_counting_handle;
};

TEST_F(StaticSharedMutexTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(kMaxReaders, kMaxReaders, _))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::sa::shared_mutex sm;
}

TEST_F(StaticSharedMutexTest, ExclusiveLockAndUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(kMaxReaders, kMaxReaders, _))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::sa::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  sm.lock();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .Times(kMaxReaders)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock();
}

TEST_F(StaticSharedMutexTest, SharedLockAndUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(kMaxReaders, kMaxReaders, _))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::sa::shared_mutex sm;

  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  sm.lock_shared();

  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting_handle))
      .WillOnce(Return(pdTRUE));
  sm.unlock_shared();
}

TEST_F(StaticSharedMutexTest, NativeHandle) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(kMaxReaders, kMaxReaders, _))
      .WillOnce(Return(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::sa::shared_mutex sm;
  EXPECT_EQ(sm.native_handle(), mock_mutex_handle);
}

#endif // configSUPPORT_STATIC_ALLOCATION
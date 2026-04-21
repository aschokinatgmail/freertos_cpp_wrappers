#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <stream_buffer.h>
#include <message_buffer.h>

#include "freertos_sw_timer.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_task.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;

namespace {
void empty_task_routine() {}
}

// ================================================================
// QUEUE BRANCH TESTS — additional template instantiations
// ================================================================

class QueueBranch4Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0xA001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// da::queue<1, int>
TEST_F(QueueBranch4Test, DaQueue1IntNamedDestructor) {
  const char *name = "q1i";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<1, int> q(name);
}

TEST_F(QueueBranch4Test, DaQueue1IntUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<1, int> q(nullptr);
}

TEST_F(QueueBranch4Test, DaQueue1IntMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<1, int> a(nullptr);
  auto b = std::move(a);
}

// da::queue<2, int>
TEST_F(QueueBranch4Test, DaQueue2IntNamedDestructor) {
  const char *name = "q2i";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<2, int> q(name);
}

TEST_F(QueueBranch4Test, DaQueue2IntUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<2, int> q(nullptr);
}

TEST_F(QueueBranch4Test, DaQueue2IntMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<2, int> a(nullptr);
  auto b = std::move(a);
}

// da::queue<4, int>
TEST_F(QueueBranch4Test, DaQueue4IntNamedDestructor) {
  const char *name = "q4i";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<4, int> q(name);
}

TEST_F(QueueBranch4Test, DaQueue4IntUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<4, int> q(nullptr);
}

TEST_F(QueueBranch4Test, DaQueue4IntMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA030);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<4, int> a(nullptr);
  auto b = std::move(a);
}

// da::queue<6, double>
TEST_F(QueueBranch4Test, DaQueue6DoubleNamedDestructor) {
  const char *name = "q6d";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<6, double> q(name);
}

TEST_F(QueueBranch4Test, DaQueue6DoubleUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<6, double> q(nullptr);
}

TEST_F(QueueBranch4Test, DaQueue6DoubleMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA040);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<6, double> a(nullptr);
  auto b = std::move(a);
}

// da::queue<8, int>
TEST_F(QueueBranch4Test, DaQueue8IntNamedDestructor) {
  const char *name = "q8i";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<8, int> q(name);
}

TEST_F(QueueBranch4Test, DaQueue8IntUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<8, int> q(nullptr);
}

TEST_F(QueueBranch4Test, DaQueue8IntMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA050);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<8, int> a(nullptr);
  auto b = std::move(a);
}

// sa::queue<4, int>
TEST_F(QueueBranch4Test, SaQueue4IntNamedDestructor) {
  const char *name = "sa4i";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<4, int> q(name);
}

TEST_F(QueueBranch4Test, SaQueue4IntUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<4, int> q(nullptr);
}

TEST_F(QueueBranch4Test, SaQueue4IntMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA060);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<4, int> a(nullptr);
  auto b = std::move(a);
}

// sa::queue<4, uint32_t>
TEST_F(QueueBranch4Test, SaQueue4Uint32NamedDestructor) {
  const char *name = "sa4u";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<4, uint32_t> q(name);
}

TEST_F(QueueBranch4Test, SaQueue4Uint32UnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<4, uint32_t> q(nullptr);
}

TEST_F(QueueBranch4Test, SaQueue4Uint32MovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA070);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<4, uint32_t> a(nullptr);
  auto b = std::move(a);
}

// sa::queue<8, bool>
TEST_F(QueueBranch4Test, SaQueue8BoolNamedDestructor) {
  const char *name = "sa8b";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<8, bool> q(name);
}

TEST_F(QueueBranch4Test, SaQueue8BoolUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<8, bool> q(nullptr);
}

TEST_F(QueueBranch4Test, SaQueue8BoolMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA080);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<8, bool> a(nullptr);
  auto b = std::move(a);
}

// sa::queue<16, char>
TEST_F(QueueBranch4Test, SaQueue16CharNamedDestructor) {
  const char *name = "sa16c";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<16, char> q(name);
}

TEST_F(QueueBranch4Test, SaQueue16CharUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<16, char> q(nullptr);
}

TEST_F(QueueBranch4Test, SaQueue16CharMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0xA090);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<16, char> a(nullptr);
  auto b = std::move(a);
}

// ================================================================
// STREAM BUFFER BRANCH TESTS — additional template instantiations
// ================================================================

class StreamBufferBranch4Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0xB001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(StreamBufferBranch4Test, DaStreamBuffer128NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<128> sb(1);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer128MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<128> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer256NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<256> sb(1);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer256MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<256> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer512NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<512> sb(1);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer512MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB030);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<512> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer1024NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<1024> sb(1);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer1024MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB040);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<1024> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer2048NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<2048> sb(1);
}

TEST_F(StreamBufferBranch4Test, DaStreamBuffer2048MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB050);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<2048> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, SaStreamBuffer64NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::sa::stream_buffer<64> sb(1);
}

TEST_F(StreamBufferBranch4Test, SaStreamBuffer64MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB060);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::sa::stream_buffer<64> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch4Test, SaStreamBuffer100NormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::sa::stream_buffer<100> sb(1);
}

TEST_F(StreamBufferBranch4Test, SaStreamBuffer100MovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0xB070);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::sa::stream_buffer<100> a(1);
  auto b = std::move(a);
}

// ================================================================
// MESSAGE BUFFER BRANCH TESTS — additional template instantiations
// ================================================================

class MessageBufferBranch4Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0xC001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferBranch4Test, DaMessageBuffer128NormalDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<128> mb;
}

TEST_F(MessageBufferBranch4Test, DaMessageBuffer128MovedFromDestruction) {
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0xC010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));

  freertos::da::message_buffer<128> a;
  auto b = std::move(a);
}

TEST_F(MessageBufferBranch4Test, DaMessageBuffer256NormalDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<256> mb;
}

TEST_F(MessageBufferBranch4Test, DaMessageBuffer256MovedFromDestruction) {
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0xC020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));

  freertos::da::message_buffer<256> a;
  auto b = std::move(a);
}

TEST_F(MessageBufferBranch4Test, SaMessageBuffer64NormalDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::sa::message_buffer<64> mb;
}

TEST_F(MessageBufferBranch4Test, SaMessageBuffer64MovedFromDestruction) {
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0xC030);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));

  freertos::sa::message_buffer<64> a;
  auto b = std::move(a);
}

// ================================================================
// TASK BRANCH TESTS — start_suspended and destructor branches
// ================================================================

class TaskBranch4Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0xD001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// da::task<512> — start_suspended=true (default)
TEST_F(TaskBranch4Test, DaTask512StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, empty_task_routine, true);
}

// da::task<512> — start_suspended=false
TEST_F(TaskBranch4Test, DaTask512StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, empty_task_routine, false);
}

// da::task<512> — moved-from destruction (m_hTask=null after move)
TEST_F(TaskBranch4Test, DaTask512MovedFromDestruction) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0xD010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(t1), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(t1));

  freertos::da::task<512> a("a", 1, empty_task_routine);
  auto b = std::move(a);
}

// da::task<1024> — start_suspended=true
TEST_F(TaskBranch4Test, DaTask1024StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<1024> t("t", 1, empty_task_routine, true);
}

// da::task<1024> — start_suspended=false
TEST_F(TaskBranch4Test, DaTask1024StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<1024> t("t", 1, empty_task_routine, false);
}

// da::task<2048> — start_suspended=true
TEST_F(TaskBranch4Test, DaTask2048StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<2048> t("t", 1, empty_task_routine, true);
}

// da::task<2048> — start_suspended=false
TEST_F(TaskBranch4Test, DaTask2048StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<2048> t("t", 1, empty_task_routine, false);
}

// sa::task<256> — start_suspended=true
TEST_F(TaskBranch4Test, SaTask256StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<256> t("t", 1, empty_task_routine, true);
}

// sa::task<256> — start_suspended=false
TEST_F(TaskBranch4Test, SaTask256StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<256> t("t", 1, empty_task_routine, false);
}

// sa::task<256> — moved-from destruction
TEST_F(TaskBranch4Test, SaTask256MovedFromDestruction) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0xD020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, vTaskDelete(t1));

  freertos::sa::task<256> a("a", 1, empty_task_routine);
  auto b = std::move(a);
}

// sa::task<512> — start_suspended=true
TEST_F(TaskBranch4Test, SaTask512StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<512> t("t", 1, empty_task_routine, true);
}

// sa::task<512> — start_suspended=false
TEST_F(TaskBranch4Test, SaTask512StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<512> t("t", 1, empty_task_routine, false);
}

// sa::task<1024> — start_suspended=true
TEST_F(TaskBranch4Test, SaTask1024StartSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<1024> t("t", 1, empty_task_routine, true);
}

// sa::task<1024> — start_suspended=false
TEST_F(TaskBranch4Test, SaTask1024StartSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _)).WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<1024> t("t", 1, empty_task_routine, false);
}
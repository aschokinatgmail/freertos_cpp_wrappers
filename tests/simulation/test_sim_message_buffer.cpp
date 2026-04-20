#include <freertos.hpp>
#include <gtest/gtest.h>

extern "C" {
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxIdleTaskStackBuffer,
                                     configSTACK_DEPTH_TYPE *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxIdleTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
}

struct MBData {
  freertos::da::message_buffer<128> mb;
  volatile bool send_ok;
  volatile bool recv_ok;
  volatile size_t recv_len;
  char recv_buf[64];
};

struct MBResults {
  volatile bool send_receive;
  volatile bool message_boundary;
  volatile bool blocking_recv;
  volatile bool data_integrity;
  volatile bool all_done;
};

static MBData s_mb_data{};
static MBResults s_mb{};

void mb_sender(void *param) {
  auto *d = static_cast<MBData *>(param);
  const char *msg = "hello freertos";
  size_t sent = d->mb.send(msg, 15, pdMS_TO_TICKS(100));
  d->send_ok = (sent == 15);
  vTaskDelete(nullptr);
}

void mb_receiver(void *param) {
  auto *d = static_cast<MBData *>(param);
  d->recv_len = d->mb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500));
  d->recv_ok = (d->recv_len > 0);
  vTaskDelete(nullptr);
}

void mb_orchestrator(void *param) {
  auto *r = static_cast<MBResults *>(param);

  r->send_receive = false;
  r->message_boundary = false;
  r->blocking_recv = false;
  r->data_integrity = false;

  {
    freertos::da::message_buffer<128> mb;
    const char *msg = "test";
    size_t sent = mb.send(msg, 5, pdMS_TO_TICKS(10));
    char buf[32] = {};
    size_t len = mb.receive(buf, 32, pdMS_TO_TICKS(10));
    r->send_receive = (sent == 5) && (len == 5) && (buf[0] == 't');
  }

  {
    freertos::da::message_buffer<128> mb;
    mb.send("abc", 4, pdMS_TO_TICKS(10));
    mb.send("xyz", 4, pdMS_TO_TICKS(10));
    char buf1[16] = {};
    char buf2[16] = {};
    size_t len1 = mb.receive(buf1, 16, pdMS_TO_TICKS(10));
    size_t len2 = mb.receive(buf2, 16, pdMS_TO_TICKS(10));
    r->message_boundary = (len1 == 4) && (len2 == 4) &&
                          (buf1[0] == 'a') && (buf2[0] == 'x');
  }

  vTaskDelay(pdMS_TO_TICKS(500));
  r->blocking_recv = s_mb_data.send_ok && s_mb_data.recv_ok &&
                     (s_mb_data.recv_len == 15) &&
                     (s_mb_data.recv_buf[0] == 'h');

  {
    freertos::da::message_buffer<256> mb;
    const char *data = "ABCDEFGHIJ0123456789";
    size_t data_len = 20;
    mb.send(data, data_len, pdMS_TO_TICKS(10));
    char buf[32] = {};
    size_t len = mb.receive(buf, 32, pdMS_TO_TICKS(10));
    r->data_integrity = (len == data_len) && (buf[0] == 'A') && (buf[19] == '9');
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimMessageBuffer, AllMessageBufferTests) {
  s_mb_data.send_ok = false;
  s_mb_data.recv_ok = false;
  s_mb_data.recv_len = 0;
  memset(&s_mb, 0, sizeof(s_mb));

  xTaskCreate(mb_sender, "mbsnd", 512, &s_mb_data, 3, nullptr);
  xTaskCreate(mb_receiver, "mbrcv", 512, &s_mb_data, 2, nullptr);
  xTaskCreate(mb_orchestrator, "mborch", 2048, &s_mb, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_mb.send_receive) << "Message buffer send/receive";
  EXPECT_TRUE(s_mb.message_boundary) << "Message buffer boundaries";
  EXPECT_TRUE(s_mb.blocking_recv) << "Message buffer blocking recv";
  EXPECT_TRUE(s_mb.data_integrity) << "Message buffer data integrity";
  EXPECT_TRUE(s_mb.all_done) << "All tests completed";
}
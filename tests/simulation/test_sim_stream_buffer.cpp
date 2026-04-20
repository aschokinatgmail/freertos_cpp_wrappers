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

struct SBData {
  freertos::da::stream_buffer<128> sb;
  volatile bool send_ok;
  volatile bool recv_ok;
  volatile size_t recv_len;
  uint8_t recv_buf[64];
};

struct SBResults {
  volatile bool send_receive;
  volatile bool partial_read;
  volatile bool blocking_on_empty;
  volatile bool trigger_level;
  volatile bool all_done;
};

static SBData s_sb_data;
static SBResults s_sb;

void sb_sender(void *param) {
  auto *d = static_cast<SBData *>(param);
  const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
  d->send_ok = (d->sb.send(data, 4, pdMS_TO_TICKS(100)) > 0);
  vTaskDelete(nullptr);
}

void sb_receiver(void *param) {
  auto *d = static_cast<SBData *>(param);
  d->recv_len = d->sb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500));
  d->recv_ok = (d->recv_len == 4);
  vTaskDelete(nullptr);
}

void sb_orchestrator(void *param) {
  auto *r = static_cast<SBResults *>(param);

  r->send_receive = false;
  r->partial_read = false;
  r->blocking_on_empty = false;
  r->trigger_level = false;

  {
    freertos::da::stream_buffer<128> sb;
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    sb.send(data, 4, pdMS_TO_TICKS(10));
    uint8_t buf[16] = {};
    size_t len = sb.receive(buf, 16, pdMS_TO_TICKS(10));
    r->send_receive = (len == 4) && (buf[0] == 0x01) && (buf[3] == 0x04);
  }

  {
    freertos::da::stream_buffer<128> sb;
    const uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    sb.send(data, 6, pdMS_TO_TICKS(10));
    uint8_t buf[16] = {};
    size_t len = sb.receive(buf, 3, pdMS_TO_TICKS(10));
    r->partial_read = (len == 3) && (buf[0] == 0xAA) && (buf[2] == 0xCC);
  }

  vTaskDelay(pdMS_TO_TICKS(500));
  r->blocking_on_empty = s_sb_data.send_ok && s_sb_data.recv_ok &&
                         (s_sb_data.recv_len == 4) &&
                         (s_sb_data.recv_buf[0] == 0xDE) &&
                         (s_sb_data.recv_buf[3] == 0xEF);

  {
    freertos::da::stream_buffer<128> sb;
    volatile bool triggered = false;
    struct P { freertos::da::stream_buffer<128> *sb; volatile bool *triggered; };
    P p{&sb, &triggered};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      uint8_t buf[16] = {};
      size_t len = pp->sb->receive(buf, 16, pdMS_TO_TICKS(1000));
      *pp->triggered = (len >= 3);
      vTaskDelete(nullptr);
    };
    sb.set_trigger_level(3);
    xTaskCreate(waiter, "sbwt", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    const uint8_t data[] = {0x10, 0x20, 0x30};
    sb.send(data, 3, pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(100));
    r->trigger_level = triggered;
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimStreamBuffer, AllStreamBufferTests) {
  memset(&s_sb, 0, sizeof(s_sb));

  xTaskCreate(sb_sender, "sbsnd", 512, &s_sb_data, 3, nullptr);
  xTaskCreate(sb_receiver, "sbrcv", 512, &s_sb_data, 2, nullptr);
  xTaskCreate(sb_orchestrator, "sborch", 2048, &s_sb, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_sb.send_receive) << "Stream buffer send/receive";
  EXPECT_TRUE(s_sb.partial_read) << "Stream buffer partial read";
  EXPECT_TRUE(s_sb.blocking_on_empty) << "Stream buffer blocking on empty";
  EXPECT_TRUE(s_sb.trigger_level) << "Stream buffer trigger level";
  EXPECT_TRUE(s_sb.all_done) << "All tests completed";
}
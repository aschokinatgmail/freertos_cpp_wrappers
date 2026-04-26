#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct MBData {
  freertos::da::message_buffer<128> mb;
  std::atomic<bool> send_ok;
  std::atomic<bool> recv_ok;
  std::atomic<size_t> recv_len;
  char recv_buf[64];

  void reset() {
    send_ok.store(false);
    recv_ok.store(false);
    recv_len.store(0);
  }
};

struct MBResults {
  std::atomic<bool> send_receive;
  std::atomic<bool> message_boundary;
  std::atomic<bool> blocking_recv;
  std::atomic<bool> data_integrity;
  std::atomic<bool> all_done;

  void reset() {
    send_receive.store(false);
    message_boundary.store(false);
    blocking_recv.store(false);
    data_integrity.store(false);
    all_done.store(false);
  }
};

static MBData s_mb_data;
static MBResults s_mb;

void mb_sender(void *param) {
  auto *d = static_cast<MBData *>(param);
  const char *msg = "hello freertos";
  size_t sent = d->mb.send(msg, 15, pdMS_TO_TICKS(100));
  d->send_ok.store(sent == 15);
  vTaskDelete(nullptr);
}

void mb_receiver(void *param) {
  auto *d = static_cast<MBData *>(param);
  d->recv_len.store(d->mb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500)));
  d->recv_ok.store(d->recv_len > 0);
  vTaskDelete(nullptr);
}

void mb_orchestrator(void *param) {
  auto *r = static_cast<MBResults *>(param);

  r->send_receive.store(false);
  r->message_boundary.store(false);
  r->blocking_recv.store(false);
  r->data_integrity.store(false);

  {
    freertos::da::message_buffer<128> mb;
    const char *msg = "test";
    size_t sent = mb.send(msg, 5, pdMS_TO_TICKS(10));
    char buf[32] = {};
    size_t len = mb.receive(buf, 32, pdMS_TO_TICKS(10));
    r->send_receive.store((sent == 5) && (len == 5) && (buf[0] == 't'));
  }

  {
    freertos::da::message_buffer<128> mb;
    mb.send("abc", 4, pdMS_TO_TICKS(10));
    mb.send("xyz", 4, pdMS_TO_TICKS(10));
    char buf1[16] = {};
    char buf2[16] = {};
    size_t len1 = mb.receive(buf1, 16, pdMS_TO_TICKS(10));
    size_t len2 = mb.receive(buf2, 16, pdMS_TO_TICKS(10));
    r->message_boundary.store((len1 == 4) && (len2 == 4) &&
                              (buf1[0] == 'a') && (buf2[0] == 'x'));
  }

  vTaskDelay(pdMS_TO_TICKS(500));
  r->blocking_recv.store(s_mb_data.send_ok && s_mb_data.recv_ok &&
                         (s_mb_data.recv_len == 15) &&
                         (s_mb_data.recv_buf[0] == 'h'));

  {
    freertos::da::message_buffer<256> mb;
    const char *data = "ABCDEFGHIJ0123456789";
    size_t data_len = 20;
    mb.send(data, data_len, pdMS_TO_TICKS(10));
    char buf[32] = {};
    size_t len = mb.receive(buf, 32, pdMS_TO_TICKS(10));
    r->data_integrity.store((len == data_len) && (buf[0] == 'A') && (buf[19] == '9'));
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimMessageBuffer, AllMessageBufferTests) {
  s_mb_data.reset();
  s_mb.reset();

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
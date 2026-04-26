#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct SBData {
  freertos::da::stream_buffer<128> sb;
  std::atomic<bool> send_ok;
  std::atomic<bool> recv_ok;
  std::atomic<size_t> recv_len;
  uint8_t recv_buf[64];

  void reset() {
    send_ok.store(false);
    recv_ok.store(false);
    recv_len.store(0);
  }
};

struct SBResults {
  std::atomic<bool> send_receive;
  std::atomic<bool> partial_read;
  std::atomic<bool> blocking_on_empty;
  std::atomic<bool> trigger_level;
  std::atomic<bool> all_done;

  void reset() {
    send_receive.store(false);
    partial_read.store(false);
    blocking_on_empty.store(false);
    trigger_level.store(false);
    all_done.store(false);
  }
};

static SBData s_sb_data;
static SBResults s_sb;

void sb_sender(void *param) {
  auto *d = static_cast<SBData *>(param);
  const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
  d->send_ok.store(d->sb.send(data, 4, pdMS_TO_TICKS(100)) > 0);
  vTaskDelete(nullptr);
}

void sb_receiver(void *param) {
  auto *d = static_cast<SBData *>(param);
  d->recv_len.store(d->sb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500)));
  d->recv_ok.store(d->recv_len == 4);
  vTaskDelete(nullptr);
}

void sb_orchestrator(void *param) {
  auto *r = static_cast<SBResults *>(param);

  r->send_receive.store(false);
  r->partial_read.store(false);
  r->blocking_on_empty.store(false);
  r->trigger_level.store(false);

  {
    freertos::da::stream_buffer<128> sb;
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    sb.send(data, 4, pdMS_TO_TICKS(10));
    uint8_t buf[16] = {};
    size_t len = sb.receive(buf, 16, pdMS_TO_TICKS(10));
    r->send_receive.store((len == 4) && (buf[0] == 0x01) && (buf[3] == 0x04));
  }

  {
    freertos::da::stream_buffer<128> sb;
    const uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    sb.send(data, 6, pdMS_TO_TICKS(10));
    uint8_t buf[16] = {};
    size_t len = sb.receive(buf, 3, pdMS_TO_TICKS(10));
    r->partial_read.store((len == 3) && (buf[0] == 0xAA) && (buf[2] == 0xCC));
  }

  vTaskDelay(pdMS_TO_TICKS(500));
  r->blocking_on_empty.store(s_sb_data.send_ok && s_sb_data.recv_ok &&
                             (s_sb_data.recv_len == 4) &&
                             (s_sb_data.recv_buf[0] == 0xDE) &&
                             (s_sb_data.recv_buf[3] == 0xEF));

  {
    freertos::da::stream_buffer<128> sb;
    std::atomic<bool> triggered{false};
    struct P { freertos::da::stream_buffer<128> *sb; std::atomic<bool> *triggered; };
    P p{&sb, &triggered};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      uint8_t buf[16] = {};
      size_t len = pp->sb->receive(buf, 16, pdMS_TO_TICKS(1000));
      pp->triggered->store(len >= 3);
      vTaskDelete(nullptr);
    };
    sb.set_trigger_level(3);
    xTaskCreate(waiter, "sbwt", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    const uint8_t data[] = {0x10, 0x20, 0x30};
    sb.send(data, 3, pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(100));
    r->trigger_level.store(triggered);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimStreamBuffer, AllStreamBufferTests) {
  s_sb.reset();
  s_sb_data.reset();

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
#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct EGData {
  freertos::da::event_group eg;
  std::atomic<bool> t1_ok;
  std::atomic<bool> t2_ok;

  void reset() {
    t1_ok.store(false);
    t2_ok.store(false);
  }
};

struct EGResults {
  std::atomic<bool> set_wait_bits;
  std::atomic<bool> sync_tasks;
  std::atomic<bool> clear_on_exit;
  std::atomic<bool> different_bits;
  std::atomic<bool> all_done;

  void reset() {
    set_wait_bits.store(false);
    sync_tasks.store(false);
    clear_on_exit.store(false);
    different_bits.store(false);
    all_done.store(false);
  }
};

static EGData s_eg_data;
static EGResults s_eg;

void eg_task1(void *param) {
  auto *d = static_cast<EGData *>(param);
  d->eg.set_bits(0x01);
  EventBits_t result = d->eg.wait_bits(0x02, pdTRUE, pdTRUE, pdMS_TO_TICKS(2000));
  d->t1_ok.store((result & 0x02) != 0);
  vTaskDelete(nullptr);
}

void eg_task2(void *param) {
  auto *d = static_cast<EGData *>(param);
  d->eg.set_bits(0x02);
  EventBits_t result = d->eg.wait_bits(0x01, pdTRUE, pdTRUE, pdMS_TO_TICKS(2000));
  d->t2_ok.store((result & 0x01) != 0);
  vTaskDelete(nullptr);
}

void eg_orchestrator(void *param) {
  auto *r = static_cast<EGResults *>(param);

  r->set_wait_bits.store(false);
  r->sync_tasks.store(false);
  r->clear_on_exit.store(false);
  r->different_bits.store(false);

  {
    freertos::da::event_group eg;
    eg.set_bits(0x05);
    EventBits_t bits = eg.wait_bits(0x05, pdFALSE, pdFALSE, pdMS_TO_TICKS(10));
    r->set_wait_bits.store((bits & 0x05) == 0x05);
  }

  vTaskDelay(pdMS_TO_TICKS(1000));
  r->sync_tasks.store(s_eg_data.t1_ok && s_eg_data.t2_ok);

  {
    freertos::da::event_group eg;
    eg.set_bits(0xFF);
    EventBits_t bits = eg.wait_bits(0x01, pdTRUE, pdTRUE, pdMS_TO_TICKS(10));
    r->clear_on_exit.store((bits & 0x01) != 0);
    EventBits_t remaining = eg.get_bits();
    r->clear_on_exit.store(r->clear_on_exit && ((remaining & 0x01) == 0) &&
                           ((remaining & 0xFE) != 0));
  }

  {
    std::atomic<bool> waiter1_ok{false};
    std::atomic<bool> waiter2_ok{false};
    freertos::da::event_group eg;
    struct P { freertos::da::event_group *eg; std::atomic<bool> *ok; EventBits_t bits; };
    P p1{&eg, &waiter1_ok, 0x01};
    P p2{&eg, &waiter2_ok, 0x02};
    auto w1 = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      EventBits_t b = pp->eg->wait_bits(pp->bits, pdTRUE, pdTRUE, pdMS_TO_TICKS(500));
      pp->ok->store((b & pp->bits) != 0);
      vTaskDelete(nullptr);
    };
    auto w2 = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      EventBits_t b = pp->eg->wait_bits(pp->bits, pdTRUE, pdTRUE, pdMS_TO_TICKS(500));
      pp->ok->store((b & pp->bits) != 0);
      vTaskDelete(nullptr);
    };
    xTaskCreate(w1, "eg1", 512, &p1, 2, nullptr);
    xTaskCreate(w2, "eg2", 512, &p2, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    eg.set_bits(0x03);
    vTaskDelay(pdMS_TO_TICKS(200));
    r->different_bits.store(waiter1_ok && waiter2_ok);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimEventGroup, AllEventGroupTests) {
  s_eg_data.reset();
  s_eg.reset();

  xTaskCreate(eg_task1, "eg1", 512, &s_eg_data, 3, nullptr);
  xTaskCreate(eg_task2, "eg2", 512, &s_eg_data, 3, nullptr);
  xTaskCreate(eg_orchestrator, "egorch", 2048, &s_eg, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_eg.set_wait_bits) << "Event group set/wait bits";
  EXPECT_TRUE(s_eg.sync_tasks) << "Event group sync tasks";
  EXPECT_TRUE(s_eg.clear_on_exit) << "Event group clear on exit";
  EXPECT_TRUE(s_eg.different_bits) << "Event group different bits";
  EXPECT_TRUE(s_eg.all_done) << "All tests completed";
}
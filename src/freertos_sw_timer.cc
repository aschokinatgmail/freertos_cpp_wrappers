#include "../include/freertos.hpp"
#include "../include/private/pool.hpp"

namespace freertos {

static pool<StaticTimer_t, FREERTOS_CPP_WRAPPERS_MAX_STATIC_SW_TIMERS>
    timer_pool;

static_sw_timer_allocator::static_sw_timer_allocator() noexcept
    : m_timer_ph(nullptr) {}
static_sw_timer_allocator::static_sw_timer_allocator(
    const static_sw_timer_allocator &src) noexcept
    : m_timer_ph{nullptr} {}
static_sw_timer_allocator::static_sw_timer_allocator(
    static_sw_timer_allocator &&src) noexcept
    : m_timer_ph{nullptr} {}
static_sw_timer_allocator::~static_sw_timer_allocator() {
  if (m_timer_ph != nullptr) {
    timer_pool.deallocate(m_timer_ph);
  }
}

static_sw_timer_allocator &static_sw_timer_allocator::operator=(
    const static_sw_timer_allocator &rhs) noexcept {
  if (this != &rhs) {
    if (m_timer_ph != nullptr) {
      timer_pool.deallocate(m_timer_ph);
      m_timer_ph = nullptr;
    }
  }
  return *this;
}
static_sw_timer_allocator &
static_sw_timer_allocator::operator=(static_sw_timer_allocator &&rhs) noexcept {
  if (this != &rhs) {
    if (m_timer_ph != nullptr) {
      timer_pool.deallocate(m_timer_ph);
      m_timer_ph = nullptr;
    }
  }
  return *this;
}

TimerHandle_t static_sw_timer_allocator::create(
    const char *name, const TickType_t period_ticks, UBaseType_t auto_reload,
    void *const timer_id, TimerCallbackFunction_t callback) {
  configASSERT(m_timer_ph == nullptr);
  m_timer_ph = timer_pool.allocate();
  if (m_timer_ph == nullptr) {
    return nullptr;
  }
  return xTimerCreateStatic(name, period_ticks, auto_reload, timer_id, callback,
                            m_timer_ph);
}

} // namespace freertos

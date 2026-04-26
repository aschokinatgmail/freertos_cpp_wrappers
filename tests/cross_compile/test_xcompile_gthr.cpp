#include "freertos_config.hpp"
#include "freertos_gthr.hpp"

void gthr_type_check() {
    __gthread_mutex_t mtx = { {}, nullptr, false };
    __gthread_recursive_mutex_t rmtx = { {}, nullptr, false };
    __gthread_cond_t cond = __GTHREAD_COND_INIT;
    __gthread_once_t once = __GTHREAD_ONCE_INIT;
    __gthread_key_t key;
    __gthread_t thread;

    (void)mtx;
    (void)rmtx;
    (void)cond;
    (void)once;

    __gthread_key_create(&key, nullptr);
    __gthread_key_delete(key);
    __gthread_setspecific(0, nullptr);
    (void)__gthread_getspecific(0);

    (void)__gthread_active_p();

#if __GTHREADS_CXX0X
    __gthread_equal(thread, thread);
    (void)__gthread_self();
    __gthread_yield();
#endif
}

int main() {
    gthr_type_check();
    while (1) {}
    return 0;
}
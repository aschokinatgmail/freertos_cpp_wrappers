#include "freertos_config.hpp"
#include "freertos_external_threading.hpp"

void et_type_check() {
    std::__libcpp_mutex_t mtx = _LIBCPP_MUTEX_INITIALIZER;
    std::__libcpp_recursive_mutex_t rmtx = { {}, nullptr, false };
    std::__libcpp_condvar_t cond = _LIBCPP_CONDVAR_INITIALIZER;
    std::__libcpp_exec_once_flag flag = _LIBCPP_EXEC_ONCE_INITIALIZER;
    std::__libcpp_thread_t thread = _LIBCPP_NULL_THREAD;

    (void)mtx;
    (void)rmtx;
    (void)cond;
    (void)flag;
    (void)thread;

    std::__libcpp_thread_id_equal(thread, thread);
    std::__libcpp_thread_isnull(thread);
}

int main() {
    et_type_check();
    while (1) {}
    return 0;
}
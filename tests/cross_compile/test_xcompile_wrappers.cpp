#include "freertos_config.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_event_group.hpp"
#include "freertos_strong_types.hpp"
#include "freertos_isr_result.hpp"

void wrapper_type_check() {
    freertos::binary_semaphore<> bin_sem;
    freertos::counting_semaphore<> cnt_sem{5};
    freertos::queue<4, int> q;
    freertos::stream_buffer<64> sb;
    freertos::message_buffer<64> mb;
    freertos::event_group<> eg;

    (void)bin_sem;
    (void)cnt_sem;
    (void)q;
    (void)sb;
    (void)mb;
    (void)eg;
}

int main() {
    wrapper_type_check();
    while (1) {}
    return 0;
}
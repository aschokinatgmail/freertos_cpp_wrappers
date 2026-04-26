#include <stdint.h>

void Reset_Handler(void);

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

int main(void) {
    while (1) {}
    return 0;
}

__attribute__((section(".isr_vector")))
void (*const g_pfnVectors[])(void) = {
    (void (*)(void))(0x20000000 + 128 * 1024),
    Reset_Handler,
};

void Reset_Handler(void) {
    for (uint32_t *p = &__bss_start__; p < &__bss_end__; p++) {
        *p = 0;
    }
    main();
    while (1) {}
}
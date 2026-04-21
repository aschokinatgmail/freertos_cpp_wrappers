#include <stdint.h>
#include <assert.h>
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

extern int main(void);
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

#define CMSDK_UART0_BASE    0x40004000UL
#define CMSDK_UART0_DATA    (*((volatile uint32_t *)(CMSDK_UART0_BASE + 0x000)))
#define CMSDK_UART0_STATE   (*((volatile uint32_t *)(CMSDK_UART0_BASE + 0x004)))
#define CMSDK_UART0_CTRL    (*((volatile uint32_t *)(CMSDK_UART0_BASE + 0x008)))
#define CMSDK_UART0_INTSTAT (*((volatile uint32_t *)(CMSDK_UART0_BASE + 0x00C)))
#define CMSDK_UART0_BAUDDIV (*((volatile uint32_t *)(CMSDK_UART0_BASE + 0x010)))

void uart_init(void) {
    CMSDK_UART0_BAUDDIV = 1;
    CMSDK_UART0_CTRL = 0x01;
}

void uart_putc(char c) {
    while (CMSDK_UART0_STATE & 0x02) {}
    CMSDK_UART0_DATA = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

void Reset_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("HardFault_Handler")));
void HardFault_Handler(void);
void MemManage_Handler(void) __attribute__((weak, alias("HardFault_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("HardFault_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("HardFault_Handler")));
void SVC_Handler(void);
void DebugMon_Handler(void) __attribute__((weak, alias("HardFault_Handler")));
void PendSV_Handler(void);
void SysTick_Handler(void);

__attribute__((section(".isr_vector")))
void (*const g_pfnVectors[])(void) = {
    (void (*)(void))(&_estack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
};

void Reset_Handler(void) {
    for (uint32_t *dst = &_sdata, *src = &_sidata; dst < &_edata;) {
        *dst++ = *src++;
    }
    for (uint32_t *dst = &_sbss; dst < &_ebss; dst++) {
        *dst = 0;
    }
    uart_init();
    uart_puts("Reset OK\r\n");
    main();
    while (1) {}
}

static void uart_put_hex(uint32_t val) {
    const char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex[(val >> i) & 0xF]);
    }
}

void HardFault_Handler(void) {
    uint32_t cfsr = *((volatile uint32_t *)0xE000ED28);
    uint32_t hfsr = *((volatile uint32_t *)0xE000ED2C);
    uint32_t mmfar = *((volatile uint32_t *)0xE000ED34);
    uint32_t bfar = *((volatile uint32_t *)0xE000ED38);
    uart_puts("HARDFAULT CFSR=");
    uart_put_hex(cfsr);
    uart_puts(" HFSR=");
    uart_put_hex(hfsr);
    uart_puts(" MMFAR=");
    uart_put_hex(mmfar);
    uart_puts(" BFAR=");
    uart_put_hex(bfar);
    uart_puts("\r\n");
    while (1) {}
}

void SVC_Handler(void) { vPortSVCHandler(); }
void PendSV_Handler(void) { xPortPendSVHandler(); }

void SysTick_Handler(void) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName;
    while (1) {}
}

void vApplicationMallocFailedHook(void) { while (1) {} }

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[128];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = 128;
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[256];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = 256;
}
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "uart.h"

static SemaphoreHandle_t all_done_sem;
static volatile int tests_remaining = 5;

static void check_done(void) {
    if (__sync_sub_and_fetch(&tests_remaining, 1) == 0) {
        xSemaphoreGive(all_done_sem);
    }
}

static void test_binary_semaphore(void *pv) {
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    xSemaphoreGive(sem);
    if (xSemaphoreTake(sem, pdMS_TO_TICKS(1000)) == pdTRUE) {
        uart_puts("PASS:binary_semaphore\r\n");
    } else {
        uart_puts("FAIL:binary_semaphore\r\n");
    }
    vSemaphoreDelete(sem);
    check_done();
    vTaskDelete(NULL);
}

static void test_mutex(void *pv) {
    SemaphoreHandle_t mtx = xSemaphoreCreateMutex();
    if (xSemaphoreTake(mtx, pdMS_TO_TICKS(1000)) == pdTRUE) {
        uart_puts("PASS:mutex\r\n");
        xSemaphoreGive(mtx);
    } else {
        uart_puts("FAIL:mutex\r\n");
    }
    vSemaphoreDelete(mtx);
    check_done();
    vTaskDelete(NULL);
}

static void test_queue(void *pv) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    int val = 42;
    xQueueSend(q, &val, pdMS_TO_TICKS(1000));
    int recv;
    if (xQueueReceive(q, &recv, pdMS_TO_TICKS(1000)) == pdTRUE && recv == 42) {
        uart_puts("PASS:queue\r\n");
    } else {
        uart_puts("FAIL:queue\r\n");
    }
    vQueueDelete(q);
    check_done();
    vTaskDelete(NULL);
}

static void test_event_group(void *pv) {
    EventGroupHandle_t eg = xEventGroupCreate();
    xEventGroupSetBits(eg, 0x01);
    EventBits_t bits = xEventGroupWaitBits(eg, 0x01, pdTRUE, pdFALSE, pdMS_TO_TICKS(1000));
    if (bits & 0x01) {
        uart_puts("PASS:event_group\r\n");
    } else {
        uart_puts("FAIL:event_group\r\n");
    }
    vEventGroupDelete(eg);
    check_done();
    vTaskDelete(NULL);
}

static void test_task_priority(void *pv) {
    if (uxTaskPriorityGet(NULL) == 2) {
        uart_puts("PASS:task_priority\r\n");
    } else {
        uart_puts("FAIL:task_priority\r\n");
    }
    check_done();
    vTaskDelete(NULL);
}

static void test_supervisor(void *pv) {
    if (xSemaphoreTake(all_done_sem, pdMS_TO_TICKS(5000)) == pdTRUE) {
        uart_puts("ALL TESTS PASSED\r\n");
    } else {
        uart_puts("TIMEOUT:tests_did_not_complete\r\n");
    }
    uart_puts("SUCCESS\r\n");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void) {
    uart_puts("=== QEMU ARM Cortex-M3 FreeRTOS Test ===\r\n");

    all_done_sem = xSemaphoreCreateCounting(1, 0);

    xTaskCreate(test_supervisor, "sup", 256, NULL, 1, NULL);
    xTaskCreate(test_binary_semaphore, "sem", 256, NULL, 2, NULL);
    xTaskCreate(test_mutex, "mtx", 256, NULL, 2, NULL);
    xTaskCreate(test_queue, "que", 256, NULL, 2, NULL);
    xTaskCreate(test_event_group, "evt", 256, NULL, 2, NULL);
    xTaskCreate(test_task_priority, "pri", 256, NULL, 2, NULL);

    uart_puts("Starting scheduler...\r\n");
    vTaskStartScheduler();

    uart_puts("ERROR:scheduler_exited\r\n");
    while (1) {}
}
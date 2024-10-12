#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#include <stdio.h>
#include "pico/stdlib.h"


static void exampleTask(void* parameters) {
    (void)parameters;

    for (;;) {
        printf("Hello, world!\n");
        vTaskDelay(1000);
    }
}

static void prvSetupHardware(void) { stdio_init_all(); }

int main() {
    prvSetupHardware();


    static TaskHandle_t exampleTaskHandle;

    printf("Example FreeRTOS Project STATS\n");

    if (xTaskCreate(exampleTask, "Example Task", configMINIMAL_STACK_SIZE, NULL,
                    tskIDLE_PRIORITY + 1, &exampleTaskHandle) != pdPASS) {
        printf("Task not created\n");
    }

    /* Start the scheduler. */
    vTaskStartScheduler();

    return 0;
}

void vApplicationMallocFailedHook(void) { configASSERT((volatile void*)NULL); }


void vApplicationIdleHook(void) {
    volatile size_t xFreeHeapSpace;

    xFreeHeapSpace = xPortGetFreeHeapSize();

    (void)xFreeHeapSpace;
}

#if (configCHECK_FOR_STACK_OVERFLOW > 0)

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    /* Check pcTaskName for the name of the offending task,
     * or pxCurrentTCB if pcTaskName has itself been corrupted. */
    (void)xTask;
    (void)pcTaskName;
}

#endif /* #if ( configCHECK_FOR_STACK_OVERFLOW > 0 ) */
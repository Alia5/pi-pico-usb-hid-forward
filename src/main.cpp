#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "pio_usb.h"
#include "tusb.h"
#include "usb/usb_descriptors.h"

#include "usb/device/hid.hpp"

static void exampleTask(void* parameters) {
    (void)parameters;

    for (;;) {
        // printf("Hello, world!\n");
        // tud_cdc_write_str("CDC Hello, world!\n");
        vTaskDelay(1000);
    }
}

static void usbHostTask(void* parameters) {
    (void)parameters;

    vTaskDelay(500);

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = 2; // TODO
    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);


    tuh_init(1);

    for (;;) {
        tuh_task();
        vTaskDelay(1);
    }
}

static void usbDeviceTask(void* parameters) {
    (void)parameters;

    tud_init(0);


    for (;;) {
        tud_task(); // tinyusb device task
        tud_cdc_write_flush();
        hid_task();
        vTaskDelay(1);
    }
}

static void prvSetupHardware() { stdio_init_all(); }
int main() {
    prvSetupHardware();


    static TaskHandle_t usbHostTaskHandle;
    static TaskHandle_t usbDeviceTaskHandle;

    printf("Pico USB-HID forwarder\n");
    printf("Starting scheduler...\n");


    if (xTaskCreate(usbHostTask, "USB Host Task", configMINIMAL_STACK_SIZE, nullptr,
                    configMAX_PRIORITIES - 2, &usbHostTaskHandle) != pdPASS) {
        printf("USB-Host Task not created\n");
    }

    UBaseType_t uxCoreAffinityMask;
    uxCoreAffinityMask = (1 << 1);
    vTaskCoreAffinitySet(usbHostTaskHandle, uxCoreAffinityMask);

    if (xTaskCreate(usbDeviceTask, "USB Device Task", configMINIMAL_STACK_SIZE, nullptr,
                    tskIDLE_PRIORITY + 3, &usbDeviceTaskHandle) != pdPASS) {
        printf("USB-Device Task not created\n");
    }
    uxCoreAffinityMask = (1 << 0);
    vTaskCoreAffinitySet(usbDeviceTaskHandle, uxCoreAffinityMask);

    if (xTaskCreate(exampleTask, "Example Task", configMINIMAL_STACK_SIZE, nullptr,
                    tskIDLE_PRIORITY + 1, nullptr) != pdPASS) {
        printf("Task not created\n");
    }

    /* Start the scheduler. */
    vTaskStartScheduler();

    return 0;
}

void vApplicationMallocFailedHook() { configASSERT((volatile void*)nullptr); }


void vApplicationIdleHook() {
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
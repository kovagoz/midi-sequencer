// task_monitor.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "task_monitor"
#define STACK_LOG_INTERVAL_MS 10000  // 10 seconds

static void task_monitor_task(void *arg)
{
    char *task_list_buffer = malloc(1024);

    if (!task_list_buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for task list");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        vTaskList(task_list_buffer);
        ESP_LOGI(TAG, "\nTask          State Prio Stack Num\n%s", task_list_buffer);
        vTaskDelay(pdMS_TO_TICKS(STACK_LOG_INTERVAL_MS));
    }

    // Should never reach here
    free(task_list_buffer);
    vTaskDelete(NULL);
}

void task_monitor_start(void)
{
    xTaskCreatePinnedToCore(
        task_monitor_task,
        "task_monitor",
        4096,
        NULL,
        tskIDLE_PRIORITY,
        NULL,
        tskNO_AFFINITY
    );
}
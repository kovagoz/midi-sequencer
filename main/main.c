#include <stdio.h>
#include "esp_event.h"
#include "controller.h"
#include "led_bar.h"
#include "sequencer.h"

static esp_event_loop_handle_t app_event_loop;

static void create_event_loop()
{
    esp_event_loop_args_t loop_args = {
        .queue_size = 8,
        .task_name = "app_event_loop_task",
        .task_priority = 1,
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &app_event_loop));
}

void app_main(void)
{
    create_event_loop();

    led_bar_init(app_event_loop);
    controller_init(app_event_loop);
    sequencer_init(app_event_loop);
}

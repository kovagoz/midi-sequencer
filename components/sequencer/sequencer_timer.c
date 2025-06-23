#include "sequencer_timer.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static gptimer_handle_t gptimer = NULL;
static TaskHandle_t *target_task_handle = NULL;
static bool is_timer_running = false;

static bool IRAM_ATTR sequencer_timer_callback(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata,
    void *user_ctx
) {
    BaseType_t task_woken = pdFALSE;
    TaskHandle_t *task_ptr = (TaskHandle_t *) user_ctx;

    vTaskNotifyGiveFromISR(*task_ptr, &task_woken);

    return task_woken == pdTRUE;
}

void sequencer_timer_init(TaskHandle_t *task_handle)
{
    assert(gptimer == NULL);
    assert(task_handle != NULL);

    target_task_handle = task_handle;

    gptimer_config_t config = {
        .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
        .direction     = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 µs resolution
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&config, &gptimer));
}

void sequencer_timer_set_period(uint32_t step_duration_ms)
{
    gptimer_alarm_config_t alarm_config = {
        .alarm_count                = step_duration_ms * 1000ULL,
        .reload_count               = 0,
        .flags.auto_reload_on_alarm = true,
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
}

void sequencer_timer_start(uint32_t step_duration_ms)
{
    assert(gptimer != NULL);
    assert(is_timer_running == false);

    sequencer_timer_set_period(step_duration_ms);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = sequencer_timer_callback,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, (void *) target_task_handle));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    is_timer_running = true;
}

void sequencer_timer_stop()
{
    assert(gptimer != NULL);
    assert(is_timer_running == true);

    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gptimer));

    is_timer_running = false;
}

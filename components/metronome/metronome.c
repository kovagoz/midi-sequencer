#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "metronome.h"

static gptimer_handle_t gptimer = NULL;
static TaskHandle_t *target_task_handle = NULL;
static bool is_timer_running = false;
static uint8_t tempo; // bpm

static bool IRAM_ATTR metronome_callback(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata,
    void *user_ctx
) {
    BaseType_t task_woken = pdFALSE;
    TaskHandle_t *task_ptr = (TaskHandle_t *) user_ctx;

    if (task_ptr != NULL) {
        vTaskNotifyGiveFromISR(*task_ptr, &task_woken);
    }

    return task_woken == pdTRUE;
}

void metronome_init()
{
    assert(gptimer == NULL);

    gptimer_config_t config = {
        .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
        .direction     = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 µs resolution
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&config, &gptimer));

    metronome_set_tempo(100);
}

void metronome_notify(TaskHandle_t *task_handle)
{
    assert(task_handle != NULL);
    target_task_handle = task_handle;
}

uint32_t metronome_get_period_ms()
{
    return 60000 / tempo / 4; // 16th notes
}

void metronome_set_tempo(uint8_t bpm)
{
    tempo = bpm;

    gptimer_alarm_config_t alarm_config = {
        .alarm_count                = metronome_get_period_ms() * 1000ULL,
        .reload_count               = 0,
        .flags.auto_reload_on_alarm = true,
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
}

void metronome_incr_tempo()
{
    if (tempo < UINT8_MAX) {
        metronome_set_tempo(tempo + 1);
    }
}

void metronome_decr_tempo()
{
    if (tempo > 2) {
        metronome_set_tempo(tempo - 1);
    }
}

void metronome_start()
{
    assert(gptimer != NULL);
    assert(is_timer_running == false);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = metronome_callback,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, (void *) target_task_handle));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    is_timer_running = true;
}

void metronome_stop()
{
    assert(gptimer != NULL);
    assert(is_timer_running == true);

    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gptimer));

    is_timer_running = false;
}

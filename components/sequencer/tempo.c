#include <stdint.h>
#include "controller.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "internal/tempo.h"
#include "metronome.h"

static void on_bpm_incr(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    metronome_incr_tempo();
}

static void on_bpm_decr(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    metronome_decr_tempo();
}

static void register_bpm_handler_task(void *pvParameters)
{
    controller_event_subscribe(CONTROLLER_EVENT_BPM_INCR, on_bpm_incr, NULL);
    controller_event_subscribe(CONTROLLER_EVENT_BPM_DECR, on_bpm_decr, NULL);

    vTaskDelete(NULL);
}

static void unregister_bpm_handler_task(void *pvParameters)
{
    controller_event_unsubscribe(CONTROLLER_EVENT_BPM_INCR, on_bpm_incr);
    controller_event_unsubscribe(CONTROLLER_EVENT_BPM_DECR, on_bpm_decr);

    vTaskDelete(NULL);
}

void sequencer_bpm_control_enable()
{
    xTaskCreate(register_bpm_handler_task, NULL, 2048, NULL, 5, NULL);
}

void sequencer_bpm_control_disable()
{
    xTaskCreate(unregister_bpm_handler_task, NULL, 2048, NULL, 5, NULL);
}
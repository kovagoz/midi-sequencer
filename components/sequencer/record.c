#include <stdint.h>
#include "controller.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "internal/persist.h"
#include "internal/record.h"
#include "internal/core.h"
#include "midi.h"

static const char *TAG = "sequencer";

static void on_note_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    midi_note_t note = *(midi_note_t *) data;
    // ESP_LOGI(TAG, "Recorded note %s at %d", midi_note_name(note), step_index);
    sequencer_step_set(note);
    sequencer_step_forward();
}

static void register_note_handler_task(void *pvParameters)
{
    controller_event_subscribe(CONTROLLER_EVENT_NOTE, on_note_event, NULL);
    vTaskDelete(NULL);
}

static void unregister_note_handler_task(void *pvParameters)
{
    controller_event_unsubscribe(CONTROLLER_EVENT_NOTE, on_note_event);
    vTaskDelete(NULL);
}

void sequencer_on_enter_record()
{
    sequencer_step_reset();
    // Workaround for the rule that a handler cannot be registered by another handler.
    xTaskCreate(register_note_handler_task, NULL, 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "Recording started");
}

void sequencer_on_exit_record()
{
    xTaskCreate(unregister_note_handler_task, NULL, 2048, NULL, 5, NULL);
    sequencer_save_sequence();
}
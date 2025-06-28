#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "internal/persist.h"
#include "internal/core.h"
#include "storage.h"

#define STORAGE_SEQUENCE_NS "sequence"
#define STORAGE_SEQUENCE_ID "default"

static const char *TAG = "sequencer";

static void save_sequence_task(void *pvParameters)
{
    sequencer_step_sequence_t sequence;
    sequencer_get_sequence(sequence);

    storage_save(STORAGE_SEQUENCE_NS, STORAGE_SEQUENCE_ID, &sequence, sizeof(sequence));

    vTaskDelete(NULL);
}

void sequencer_save_sequence()
{
    xTaskCreate(save_sequence_task, NULL, 2048, NULL, 5, NULL);
}

void sequencer_load_sequence()
{
    sequencer_step_sequence_t sequence;

    size_t size = sizeof(sequence);

    esp_err_t err = storage_load(STORAGE_SEQUENCE_NS, STORAGE_SEQUENCE_ID, &sequence, &size);

    if (err != ESP_OK && err != STORAGE_NOT_FOUND) {
        ESP_LOGE(TAG, "Could not load the sequence from storage");
    }

    sequencer_set_sequence(sequence);
}
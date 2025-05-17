#include <stdint.h>
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "midi/notes.h"
#include "sequencer.h"

ESP_EVENT_DEFINE_BASE(SEQUENCER_EVENT);

static uint8_t tempo = 120; // bpm
static TaskHandle_t xHandle = NULL;
static esp_event_loop_handle_t event_loop;

static void sequencer_play_task(void *pvParameters)
{
    StepSequence sequence = {
        {NOTE_C4},
        {NOTE_D4},
        {NOTE_E4},
        {NOTE_F4},
        {NOTE_G4},
        {NOTE_A4},
        {NOTE_B4},
        {NOTE_C5}
    };

    uint32_t step_duration_ms = 60000 / tempo;

    while (1) {
        for (int i = 0; i < STEP_SEQUENCE_LENGTH; i++) {
            sequencer_step_event_t ev = {
                .step = &sequence[i],
                .step_index = i
            };

            esp_event_post_to(event_loop, SEQUENCER_EVENT, SEQUENCER_EVENT_STEP, &ev, sizeof(ev), portMAX_DELAY);

            vTaskDelay(pdMS_TO_TICKS(step_duration_ms));
        }
    }
}

void sequencer_init(esp_event_loop_handle_t loop)
{
    event_loop = loop;
}

void sequencer_play()
{
    if (xHandle != NULL) {
        return;
    }

    xTaskCreate(sequencer_play_task, "sequencer_play", 2048, NULL, 5, &xHandle);
}

void sequencer_stop()
{
    if (xHandle != NULL) {
        vTaskDelete(xHandle);
    }
}
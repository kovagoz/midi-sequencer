#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "midi/midi.h"
#include "midi/notes.h"

void midi_task(void *pvParameters)
{
    uint8_t channel  = 5;
    uint8_t note     = NOTE_C4;
    uint8_t velocity = 100;

    while (true) {
        midi_note_on(channel, note, velocity);
        vTaskDelay(pdMS_TO_TICKS(300));

        midi_note_off(channel, note, 64);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI("main", "Note sent");
    }
}

void app_main(void)
{
    midi_init(UART_NUM_2, GPIO_NUM_18);

    xTaskCreate(
        midi_task,         // Task function
        "midi_task",       // Task name (for debugging)
        2048,              // Stack size in words (not bytes)
        NULL,              // Task parameter
        5,                 // Priority (higher number = higher priority)
        NULL               // Task handle (not used here)
    );
}

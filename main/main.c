#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "midi/midi.h"
#include "midi/notes.h"

#define MIDI_CH 5

void app_main(void) {
    midi_init(UART_NUM_2, GPIO_NUM_18);

    while (1) {
        midi_note_on(MIDI_CH, NOTE_C4, 100);
        vTaskDelay(pdMS_TO_TICKS(200));

        midi_note_off(MIDI_CH, NOTE_C4, 64);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI("main", "Note sent");
    }
}

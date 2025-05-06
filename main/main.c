#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "midi/midi.h"
#include "midi/notes.h"

#define MIDI_CH 4

void midi_player_task(void *pvParameters)
{
    MidiPlayerArgs *args = (MidiPlayerArgs *) pvParameters;

    const StepSequence *sequence = args->sequence;
    uint16_t tempo = args->tempo;

    uint32_t step_duration_ms = 60000 / tempo;

    while (true) {
        for (int i = 0; i < STEP_SEQUENCE_LENGTH; i++) {
            Step step = (*sequence)[i];
            uint32_t gate_time_ms = (step.gate * step_duration_ms) / 100;

            midi_note_on(MIDI_CH, step.note, step.velocity);
            vTaskDelay(pdMS_TO_TICKS(gate_time_ms));
            midi_note_off(MIDI_CH, step.note, 64);

            vTaskDelay(pdMS_TO_TICKS(step_duration_ms - gate_time_ms));
        }
    }
}

void app_main(void)
{
    midi_init(UART_NUM_2, GPIO_NUM_18);

    StepSequence melody = {
        {60, 100, 80},
        {62, 100, 80},
        {64, 100, 80},
        {65, 100, 80},
        {67, 100, 80},
        {69, 100, 80},
        {71, 100, 80},
        {72, 100, 80}
    };

    MidiPlayerArgs player_args = {
        .sequence = &melody,
        .tempo    = 120
    };

    xTaskCreate(
        midi_player_task,    // Task function
        "MIDI Player",       // Task name (for debugging)
        2048,                // Stack size in words (not bytes)
        &player_args,        // Task parameter
        5,                   // Priority (higher number = higher priority)
        NULL                 // Task handle (not used here)
    );
}

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "midi.h"
#include "sequencer.h"
#include "transmitter.h"

#define TRANSMITTER_CHANNEL 5

static void on_note_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    sequencer_step_trig_event_t trig = *(sequencer_step_trig_event_t*) data;

    // Note ON
    midi_out_note_on(TRANSMITTER_CHANNEL, trig.note, MIDI_NOTE_VELOCITY_MAX);

    // Wait according to the gate time
    vTaskDelay(pdMS_TO_TICKS(trig.gate_time_ms));

    // Note OFF
    midi_out_note_off(TRANSMITTER_CHANNEL, trig.note, 0);
}

void transmitter_init(esp_event_loop_handle_t event_loop)
{
    midi_out_init(UART_NUM_1, GPIO_NUM_25);

    esp_event_handler_register_with(
        event_loop,
        SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP_TRIG,
        on_note_event,
        NULL
    );
}
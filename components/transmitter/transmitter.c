#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "midi.h"
#include "sequencer.h"
#include "transmitter.h"

#define TRANSMITTER_CHANNEL 5

static void transmitter_send_note_task(void *pvParameters)
{
    sequencer_step_trig_event_t trig = *(sequencer_step_trig_event_t*) pvParameters;

    // Note ON
    midi_out_note_on(TRANSMITTER_CHANNEL, trig.note, MIDI_NOTE_VELOCITY_MAX);

    // Wait according to the gate time
    vTaskDelay(pdMS_TO_TICKS(trig.gate_time_ms));

    // Note OFF
    midi_out_note_off(TRANSMITTER_CHANNEL, trig.note, 0);

    vTaskDelete(NULL);
}

static void transmitter_on_note_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    // Creating a new task is the way to not block the event loop
    // with calling `vTaskDelay` (between note on and off).
    xTaskCreate(transmitter_send_note_task, NULL, 2048, data, 5, NULL);
}

void transmitter_init(esp_event_loop_handle_t event_loop)
{
    midi_out_init(UART_NUM_1, GPIO_NUM_25);
    
    // Because the NPN transistor on the output flips the signal.
    midi_out_invert_signal();

    esp_event_handler_register_with(
        event_loop,
        SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP_TRIG,
        transmitter_on_note_event,
        NULL
    );
}
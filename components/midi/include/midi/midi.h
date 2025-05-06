#ifndef _MIDI_H
#define _MIDI_H

#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"

// --- Macros -----------------------------------

#define STEP_SEQUENCE_LENGTH 8

// --- Type definitions -------------------------

typedef struct {
    uint8_t note;      // MIDI note number (0–127)
    uint8_t velocity;  // MIDI velocity (0–127)
    uint8_t gate;      // Gate length (e.g., percent or ticks relative to tempo)
} Step;

typedef Step StepSequence[STEP_SEQUENCE_LENGTH];

typedef struct {
    const StepSequence *sequence;
    uint16_t tempo;
} MidiPlayerArgs;

// --- Function signatures ----------------------

void midi_init(uart_port_t uart_num, gpio_num_t tx_pin);
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity);

#endif // _MIDI_H
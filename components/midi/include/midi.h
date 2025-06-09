#pragma once

#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "midi/notes.h"
#include "midi/parser.h"

#define MIDI_NOTE_VELOCITY_MAX 127

typedef uint8_t midi_note_t;

// --- Function signatures ----------------------

void midi_out_init(uart_port_t uart_num, gpio_num_t tx_pin);
void midi_out_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_out_note_off(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_out_invert_signal();

void midi_in_init(uart_port_t uart_num, gpio_num_t tx_pin);
void midi_in_set_handler(midi_message_handler_t handler);
void midi_in_listen();
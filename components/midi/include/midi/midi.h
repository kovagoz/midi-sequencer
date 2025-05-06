#ifndef _MIDI_H_INCLUDED
#define _MIDI_H_INCLUDED

#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"

void midi_init(uart_port_t uart_num, gpio_num_t tx_pin);
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity);

#endif // _MIDI_H_INCLUDED
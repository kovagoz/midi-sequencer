#include "midi/midi.h"

static uart_port_t midi_uart = UART_NUM_1;

void midi_init(uart_port_t uart_num, gpio_num_t tx_pin)
{
    midi_uart = uart_num;

    const uart_config_t uart_config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(
        uart_num,
        tx_pin,
        UART_PIN_NO_CHANGE, // RX
        UART_PIN_NO_CHANGE, // RTS
        UART_PIN_NO_CHANGE  // CTS
    ));
}

void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t msg[3] = {
        0x90 | (channel & 0x0F),  // Note On command + channel (0–15)
        note & 0x7F,              // Note number (0–127)
        velocity & 0x7F           // Velocity (0–127)
    };

    uart_write_bytes(midi_uart, (const char *) msg, sizeof(msg));
}

void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t msg[3] = {
        0x80 | (channel & 0x0F),   // Note Off + channel
        note & 0x7F,               // Note number (0–127)
        velocity & 0x7F            // Release velocity (0–127)
    };

    uart_write_bytes(midi_uart, (const char *) msg, sizeof(msg));
}

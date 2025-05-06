#include "midi/midi.h"

static uart_port_t midi_uart = UART_NUM_1;

/**
 * @brief Initialize the UART interface for sending MIDI messages.
 *
 * This function configures the specified UART port for standard MIDI communication
 * (31250 baud, 8N1, no flow control), and sets the TX pin to the provided GPIO.
 * It must be called before using any other MIDI functions like midi_note_on() or midi_note_off().
 *
 * @param uart_num The UART port to use (e.g., UART_NUM_1).
 * @param tx_pin   The GPIO number to use as the UART TX pin.
 */
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

/**
 * @brief Send a MIDI Note On message.
 *
 * This function sends a Note On message over the configured UART to trigger a note on the specified channel.
 * The note and velocity values will be clamped to 7-bit values (0–127).
 *
 * @param channel   The MIDI channel number (0–15).
 * @param note      The MIDI note number (0–127), where 60 = Middle C.
 * @param velocity  The velocity (0–127), which typically controls how "hard" the note is played.
 */
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t msg[3] = {
        0x90 | (channel & 0x0F),  // Note On command + channel (0–15)
        note & 0x7F,              // Note number (0–127)
        velocity & 0x7F           // Velocity (0–127)
    };

    uart_write_bytes(midi_uart, (const char *) msg, sizeof(msg));
}

/**
 * @brief Send a MIDI Note Off message.
 *
 * This function sends a Note Off message over the configured UART to stop a note on the specified channel.
 * The velocity parameter is optional and often used to indicate the release speed of the note.
 *
 * @param channel   The MIDI channel number (0–15).
 * @param note      The MIDI note number (0–127) to stop.
 * @param velocity  The release velocity (0–127). Some devices ignore this, but it's part of the MIDI spec.
 */
void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t msg[3] = {
        0x80 | (channel & 0x0F),   // Note Off + channel
        note & 0x7F,               // Note number (0–127)
        velocity & 0x7F            // Release velocity (0–127)
    };

    uart_write_bytes(midi_uart, (const char *) msg, sizeof(msg));
}

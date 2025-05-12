#include <stdio.h>
#include "driver/uart.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "midi/parser.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024

// void print_byte_binary(uint8_t byte)
// {
//     for (int i = 7; i >= 0; i--) {
//         putchar((byte >> i) & 1 ? '1' : '0');
//     }
//     putchar('\n');
// }

void handle_midi_message(const midi_message_t *msg)
{
    printf("Channel: %d, Note: %s\n", msg->data.note.channel, midi_note_name(msg->data.note.note));
}

void app_main(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200, // Not MIDI standard, but works for this test
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t data[BUF_SIZE];

    while (true) {
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE, pdMS_TO_TICKS(100));

        for (int i = 0; i < len; i++) {
            midi_parse_message(data[i], handle_midi_message);
        }
    }
}

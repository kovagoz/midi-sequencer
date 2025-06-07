#include <stdint.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "midi.h"

#define MIDI_IN_UART_BUF_SIZE 1024

static const char *TAG = "midi_in";
static uart_port_t midi_in_uart;
static midi_message_handler_t midi_in_msg_handler = NULL;
static TaskHandle_t midi_in_task_handle = NULL;

void midi_in_init(uart_port_t uart_num, gpio_num_t rx_pin)
{
    midi_in_uart = UART_NUM_0;
    // midi_in_uart = uart_num;

    // TODO extract
    const uart_config_t uart_config = {
        .baud_rate = 115200, // FIXME Not MIDI standard, but works during development
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_driver_install(midi_in_uart, MIDI_IN_UART_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(midi_in_uart, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(
        midi_in_uart,
        GPIO_NUM_1, // FIXME Set to UART_PIN_NO_CHANGE
        GPIO_NUM_3, // FIXME Use the given Rx pin
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    ));
}

void midi_in_set_handler(midi_message_handler_t handler)
{
    midi_in_msg_handler = handler;
}

/**
 * @brief Task that reads MIDI data from UART and parses messages.
 *
 * Continuously reads bytes from UART and feeds them to the MIDI parser,
 * which invokes the controller message handler upon receiving complete messages.
 *
 * @param pvParameters Unused.
 */
static void midi_in_task(void *pvParameters)
{
    uint8_t data[MIDI_IN_UART_BUF_SIZE];

    while (1) {
        int len = uart_read_bytes(midi_in_uart, data, MIDI_IN_UART_BUF_SIZE, pdMS_TO_TICKS(10));

        for (int i = 0; i < len; i++) {
            midi_parse_message(data[i], midi_in_msg_handler);
        }
    }
}

void midi_in_listen()
{
    if (midi_in_msg_handler == NULL) {
        ESP_LOGE(TAG, "MIDI message handler must be set");
    } else if (midi_in_task_handle == NULL) {
        xTaskCreate(midi_in_task, "midi_in", 2048, NULL, 1, &midi_in_task_handle);
    }
}

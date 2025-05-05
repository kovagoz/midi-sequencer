#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#define UART_NUM UART_NUM_2
#define TX_PIN 18
#define MIDI_CH 5

void app_main(void) {
    const uart_config_t uart_config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // 2. Size of Tx ring buffer
    // 3 .Size of Rx ring buffer
    // 4. Event queue handle and size
    // 5. Flags to allocate an interrupt
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(
        UART_NUM,
        TX_PIN,
        UART_PIN_NO_CHANGE, // RX
        UART_PIN_NO_CHANGE, // RTS
        UART_PIN_NO_CHANGE  // CTS
    ));

    // Send Note On: Note 60 (Middle C), Velocity 100
    uint8_t note_on[] = { 0x90 + MIDI_CH - 1, 60, 100 };

    // Send Note Off: Note 60, Velocity 0
    uint8_t note_off[] = { 0x80 + MIDI_CH - 1, 60, 0 };

    while (1) {
        uart_write_bytes(UART_NUM, (const char *) note_on, sizeof(note_on));
        vTaskDelay(pdMS_TO_TICKS(200));

        uart_write_bytes(UART_NUM, (const char *) note_off, sizeof(note_off));
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI("main", "Note sent");
    }
}

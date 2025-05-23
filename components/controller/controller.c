#include <stdbool.h>
#include "controller.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "midi.h"

#define CONTROLLER_UART_PORT UART_NUM_0
#define CONTROLLER_BUF_SIZE 1024

ESP_EVENT_DEFINE_BASE(CONTROLLER_EVENT);

static const char *TAG = "controller";
static esp_event_loop_handle_t event_loop;

/**
 * @brief Initializes the UART peripheral used for MIDI input.
 *
 * Configures UART0 with 115200 baud (for test purposes), 8 data bits,
 * no parity, and 1 stop bit. Also assigns the UART TX and RX pins.
 */
static void controller_uart_init()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200, // Not MIDI standard, but works for this test
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_driver_install(CONTROLLER_UART_PORT, CONTROLLER_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CONTROLLER_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(CONTROLLER_UART_PORT, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGD(TAG, "UART has been initialized");
}

/**
 * @brief Checks if the incoming MIDI message indicates the Play button was pressed.
 *
 * @param msg Pointer to the parsed MIDI message.
 * @return true if the message is a Control Change for controller 107 with value 127.
 * @return false otherwise.
 */
static bool controller_play_pressed(const midi_message_t *msg)
{
    return msg->type == MIDI_MSG_CC && \
        msg->data.cc.controller == 107 && \
        msg->data.cc.value == 127;
}

/**
 * @brief Checks if the incoming MIDI message indicates the Stop button was pressed.
 *
 * @param msg Pointer to the parsed MIDI message.
 * @return true if the message is a Control Change for controller 106 with value 127.
 * @return false otherwise.
 */
static bool controller_stop_pressed(const midi_message_t *msg)
{
    return msg->type == MIDI_MSG_CC && \
        msg->data.cc.controller == 106 && \
        msg->data.cc.value == 127;
}

/**
 * @brief Handles incoming MIDI messages relevant to control actions.
 *
 * Processes Play and Stop button events and posts corresponding events
 * to the application event loop.
 *
 * @param msg Pointer to the MIDI message to process.
 */
static void controller_midi_message_handler(const midi_message_t *msg)
{
    if (controller_play_pressed(msg)) {
        esp_event_post_to(event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_PLAY, NULL, 0, portMAX_DELAY);
    }

    if (controller_stop_pressed(msg)) {
        esp_event_post_to(event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_STOP, NULL, 0, portMAX_DELAY);
    }

    // printf("Channel: %d, Note: %s\n", msg->data.note.channel, midi_note_name(msg->data.note.note));
}

/**
 * @brief Task that reads MIDI data from UART and parses messages.
 *
 * Continuously reads bytes from UART and feeds them to the MIDI parser,
 * which invokes the controller message handler upon receiving complete messages.
 *
 * @param pvParameters Unused.
 */
static void controller_midi_receiver_task(void *pvParameters)
{
    ESP_LOGD(TAG, "Starting MIDI receiver task");

    uint8_t data[CONTROLLER_BUF_SIZE];

    while (1) {
        int len = uart_read_bytes(CONTROLLER_UART_PORT, data, CONTROLLER_BUF_SIZE, pdMS_TO_TICKS(100));

        for (int i = 0; i < len; i++) {
            midi_parse_message(data[i], controller_midi_message_handler);
        }
    }
}

// See documentation in controller.h
void controller_init(esp_event_loop_handle_t app_event_loop)
{
    event_loop = app_event_loop;

    controller_uart_init();

    xTaskCreate(controller_midi_receiver_task, "controller_midi_receiver", 2048, NULL, 1, NULL);
}
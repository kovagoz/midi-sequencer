#include <stdbool.h>
#include "controller.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "midi.h"

ESP_EVENT_DEFINE_BASE(CONTROLLER_EVENT);

static const char *TAG = "controller";
static esp_event_loop_handle_t event_loop;

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

static bool controller_rec_pressed(const midi_message_t *msg)
{
    return msg->type == MIDI_MSG_CC && \
        msg->data.cc.controller == 108 && \
        msg->data.cc.value == 127;
}

static bool controller_note_pressed(const midi_message_t *msg)
{
    return msg->type == MIDI_MSG_NOTE_ON;
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
    } else if (controller_stop_pressed(msg)) {
        esp_event_post_to(event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_STOP, NULL, 0, portMAX_DELAY);
    } else if (controller_rec_pressed(msg)) {
        esp_event_post_to(event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_REC, NULL, 0, portMAX_DELAY);
    } else if (controller_note_pressed(msg)) {
        esp_event_post_to(
            event_loop,
            CONTROLLER_EVENT,
            CONTROLLER_EVENT_NOTE,
            &msg->data.note.note,
            sizeof(msg->data.note.note),
            portMAX_DELAY
        );
    }

    // printf("Channel: %d, Note: %s\n", msg->data.note.channel, midi_note_name(msg->data.note.note));
}

// See documentation in controller.h
void controller_init(esp_event_loop_handle_t app_event_loop)
{
    event_loop = app_event_loop;

    midi_in_init(UART_NUM_0, GPIO_NUM_3);
    midi_in_set_handler(controller_midi_message_handler);
    midi_in_listen();
}
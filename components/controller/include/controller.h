#pragma once

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(CONTROLLER_EVENT);

typedef enum {
    CONTROLLER_EVENT_STOP,
    CONTROLLER_EVENT_PLAY,
    CONTROLLER_EVENT_REC,
    CONTROLLER_EVENT_NOTE,
} controller_event_t;

/**
 * @brief Initializes the controller component.
 *
 * Sets the application event loop handle, initializes UART, and starts
 * the MIDI receiver task.
 *
 * @param app_event_loop Event loop used to post sequencer events.
 */
void controller_init(esp_event_loop_handle_t event_loop);
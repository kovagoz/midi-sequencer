#pragma once

#include "esp_event.h"

/**
 * @brief Initializes the controller component.
 *
 * Sets the application event loop handle, initializes UART, and starts
 * the MIDI receiver task.
 *
 * @param app_event_loop Event loop used to post sequencer events.
 */
void controller_init(esp_event_loop_handle_t event_loop);
#pragma once

#include <stdint.h>
#include "esp_event.h"

void sequencer_set_event_loop(esp_event_loop_handle_t event_loop);
void sequencer_event_emit(int32_t event_id, const void *event_data, size_t event_data_size);
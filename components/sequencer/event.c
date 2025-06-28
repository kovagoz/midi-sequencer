#include "esp_event.h"
#include "internal/event.h"
#include "sequencer.h"

static esp_event_loop_handle_t sequencer_event_loop;

void sequencer_set_event_loop(esp_event_loop_handle_t event_loop)
{
    sequencer_event_loop = event_loop;
}

void sequencer_event_emit(int32_t event_id, const void *event_data, size_t event_data_size)
{
    esp_event_post_to(
        sequencer_event_loop,
        SEQUENCER_EVENT,
        event_id,
        event_data,
        event_data_size,
        portMAX_DELAY
    );
}
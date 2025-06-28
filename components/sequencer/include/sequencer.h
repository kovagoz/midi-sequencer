#pragma once

#include "esp_event.h"
#include "midi.h"
#include "time.h"

ESP_EVENT_DECLARE_BASE(SEQUENCER_EVENT);

/**
 * @brief Event identifiers for the sequencer event loop.
 *
 * Defines the types of events that the sequencer can emit.
 */
enum {
    SEQUENCER_EVENT_STATE_CHANGED,
    SEQUENCER_EVENT_STEP_SELECT,
    SEQUENCER_EVENT_STEP_TRIG,
};

/**
 * @brief Payload for a SEQUENCER_EVENT_STEP event.
 *
 * Contains information about the step that was triggered, including
 * a pointer to the step data and its index within the sequence.
 */
typedef struct {
    midi_note_t note;
    time_ms_t gate_time_ms;
} sequencer_step_trig_event_t;

// --- Function signatures ----------------------

void sequencer_init(esp_event_loop_handle_t event_loop);
void sequencer_start();
void sequencer_stop();
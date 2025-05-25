#pragma once

typedef enum {
    SEQUENCER_STATE_STOP,
    SEQUENCER_STATE_PLAY,
    SEQUENCER_STATE_REC,
    SEQUENCER_STATE_MAX,
} sequencer_state_t;

typedef struct {
    sequencer_state_t previous;
    sequencer_state_t current;
} sequencer_state_change_event_t;
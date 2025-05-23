#pragma once

#include "esp_event.h"

#define STEP_SEQUENCE_LENGTH 8

/**
 * @brief Represents a single step in a sequence.
 *
 * Each step contains information about the MIDI note to play.
 */
typedef struct {
    uint8_t note;      // MIDI note number (0–127)
    // uint8_t velocity;  // MIDI velocity (0–127)
    // uint8_t gate;      // Gate length (e.g., percent or ticks relative to tempo)
} Step;

/**
 * @brief Defines a fixed-length sequence of steps.
 *
 * A StepSequence represents one track of steps to be played
 * by the sequencer. Each step may correspond to a MIDI note
 * to trigger at a specific time in the pattern.
 */
typedef Step StepSequence[STEP_SEQUENCE_LENGTH];

/**
 * @brief Represents a complete pattern consisting of a step sequence and tempo.
 *
 * A Pattern defines the musical content and playback speed of a single track.
 * It contains a reference to a StepSequence and a tempo value that determines
 * how fast the steps are played.
 */
typedef struct {
    const StepSequence *sequence; // Pointer to the step sequence data
    uint8_t tempo;                // Tempo in beats per minute (BPM)
} Pattern;

ESP_EVENT_DECLARE_BASE(SEQUENCER_EVENT);

/**
 * @brief Event identifiers for the sequencer event loop.
 *
 * Defines the types of events that the sequencer can emit.
 */
enum {
    SEQUENCER_EVENT_STEP = 0,
    SEQUENCER_EVENT_PLAY = 1,
    SEQUENCER_EVENT_STOP = 2,
};

/**
 * @brief Payload for a SEQUENCER_EVENT_STEP event.
 *
 * Contains information about the step that was triggered, including
 * a pointer to the step data and its index within the sequence.
 */
typedef struct {
    const Step* step;
    size_t step_index;
} sequencer_step_event_t;

// --- Function signatures ----------------------

void sequencer_init();
void sequencer_play();
void sequencer_stop();
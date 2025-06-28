#pragma once

#include <stdint.h>
#include "midi.h"

#define STEP_SEQUENCE_LENGTH 8

/**
 * @brief Defines a fixed-length sequence of steps.
 *
 * A StepSequence represents one track of steps to be played
 * by the sequencer. Each step may correspond to a MIDI note
 * to trigger at a specific time in the pattern.
 */
typedef midi_note_t sequencer_step_sequence_t[STEP_SEQUENCE_LENGTH];

void sequencer_step_select(uint8_t pos);
void sequencer_step_forward();
void sequencer_step_trig();
void sequencer_step_reset();
void sequencer_step_set(midi_note_t note);

void sequencer_get_sequence(sequencer_step_sequence_t out_sequence);
void sequencer_set_sequence(sequencer_step_sequence_t new_sequence);
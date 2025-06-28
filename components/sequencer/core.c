#include <stdint.h>
#include <string.h>
#include "internal/event.h"
#include "internal/core.h"
#include "metronome.h"
#include "sequencer.h"

static uint8_t step_index = 0;

static sequencer_step_sequence_t sequence = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};

static uint8_t gate = 80; // percent

void sequencer_step_select(uint8_t pos)
{
    step_index = pos;

    sequencer_event_emit(SEQUENCER_EVENT_STEP_SELECT, &step_index, sizeof(step_index));
}

void sequencer_step_forward()
{
    sequencer_step_select((step_index + 1) % STEP_SEQUENCE_LENGTH);
}

void sequencer_step_reset()
{
    sequencer_step_select(0);
}

void sequencer_step_trig()
{
    const sequencer_step_trig_event_t event  = {
        .note = sequence[step_index],
        .gate_time_ms = metronome_get_period_ms() * gate / 100,
    };

    sequencer_event_emit(SEQUENCER_EVENT_STEP_TRIG, &event, sizeof(event));
}

void sequencer_step_set(midi_note_t note)
{
    sequence[step_index] = note;
}

void sequencer_get_sequence(sequencer_step_sequence_t out_sequence)
{
    memcpy(out_sequence, sequence, sizeof(sequence));
}

void sequencer_set_sequence(sequencer_step_sequence_t new_sequence)
{
    memcpy(sequence, new_sequence, sizeof(sequence));
}
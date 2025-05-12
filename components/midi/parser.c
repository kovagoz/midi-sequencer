#include <stdio.h>
#include <stdbool.h>
#include "midi/parser.h"

const char *midi_note_name(uint8_t note_number)
{
    static char name[4];  // Enough for "C#4" + null

    static const char *note_names[] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    uint8_t note  = note_number % 12;
    int8_t octave = (note_number / 12) - 1;

    snprintf(name, sizeof(name), "%s%d", note_names[note], octave);

    return name;
}

void midi_parse_message(uint8_t byte, midi_message_handler_t handler)
{
    static midi_parser_state_t state = MIDI_FSM_WAIT_STATUS;
    static midi_message_t message = {
        .type = MIDI_MSG_UNKNOWN
    };

    bool msg_ready = false;

    // Status byte received
    if (byte >> 7 == 1) {
        switch (byte >> 4) {
            case MIDI_MSG_NOTE_ON: // Note ON
                message.type = MIDI_MSG_NOTE_ON;
                message.data.note.channel = byte & 0x0F;
                state = MIDI_FSM_WAIT_DATA1;
                break;

            default:
                message.type = MIDI_MSG_UNKNOWN;
                break;
        }
    // Data byte received
    } else {
        switch (state) {
            // First data byte received
            case MIDI_FSM_WAIT_DATA1:
                switch (message.type) {
                    case MIDI_MSG_NOTE_ON:
                        message.data.note.note = byte & 0x7F;
                        state = MIDI_FSM_WAIT_DATA2;
                        break;

                    default:
                        break;
                }
                break;

            // Second data byte received
            case MIDI_FSM_WAIT_DATA2:
                switch (message.type) {
                    case MIDI_MSG_NOTE_ON:
                        message.data.note.velocity = byte & 0x7F;
                        msg_ready = true;
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

    if (msg_ready) {
        state = MIDI_FSM_WAIT_STATUS;
        
        if (handler != NULL) {
            handler(&message);
        }
    }
}
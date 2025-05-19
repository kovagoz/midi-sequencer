#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"
#include "midi/parser.h"

#define MIDI_MSG_CHANNEL(b) b & 0x0F
#define MIDI_MSG_DATA_VALUE(b) b & 0x7F

static const char *TAG = "MIDI parser";

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
    ESP_LOGD(TAG, "Message received");

    static midi_parser_state_t state = MIDI_FSM_WAIT_STATUS;
    static midi_message_t message = {
        .type = MIDI_MSG_UNKNOWN
    };

    bool msg_ready = false;

    // Status byte received
    if (byte >> 7 == 1) {
        switch (byte >> 4) {
            case MIDI_MSG_NOTE_ON: // Note ON
                ESP_LOGD(TAG, "Note message detected");
                message.type = MIDI_MSG_NOTE_ON;
                message.data.note.channel = MIDI_MSG_CHANNEL(byte);
                state = MIDI_FSM_WAIT_DATA1;
                break;

            case MIDI_MSG_CC:
                ESP_LOGD(TAG, "CC message detected");
                message.type = MIDI_MSG_CC;
                message.data.cc.channel = MIDI_MSG_CHANNEL(byte);
                state = MIDI_FSM_WAIT_DATA1;
                break;

            default:
                ESP_LOGD(TAG, "Unknown status byte");
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
                        message.data.note.note = MIDI_MSG_DATA_VALUE(byte);
                        state = MIDI_FSM_WAIT_DATA2;
                        break;

                    case MIDI_MSG_CC:
                        message.data.cc.controller = MIDI_MSG_DATA_VALUE(byte);
                        state = MIDI_FSM_WAIT_DATA2;
                        break;

                    default:
                        ESP_LOGD(TAG, "Unknown data byte");
                        break;
                }
                break;

            // Second data byte received
            case MIDI_FSM_WAIT_DATA2:
                switch (message.type) {
                    case MIDI_MSG_NOTE_ON:
                        message.data.note.velocity = MIDI_MSG_DATA_VALUE(byte);
                        msg_ready = true;
                        break;

                    case MIDI_MSG_CC:
                        message.data.cc.value = MIDI_MSG_DATA_VALUE(byte);
                        msg_ready = true;
                        break;

                    default:
                        ESP_LOGD(TAG, "Unknown data byte");
                        break;
                }
                break;

            default:
                break;
        }
    }

    if (msg_ready) {
        ESP_LOGD(TAG, "New message is ready");

        state = MIDI_FSM_WAIT_STATUS;
        
        if (handler != NULL) {
            ESP_LOGD(TAG, "Calling the message handler");
            handler(&message);
        }
    }
}
#ifndef _MIDI_PARSER_H
#define _MIDI_PARSER_H

#include <stdint.h>

/**
 * @brief Enumeration of supported MIDI message types.
 */
typedef enum {
    MIDI_MSG_UNKNOWN  = 0x00,
    MIDI_MSG_NOTE_ON  = 0x09,
    MIDI_MSG_NOTE_OFF = 0x08,
    MIDI_MSG_CC       = 0x0B,
} midi_message_type_t;

typedef struct {
    uint8_t channel;
    uint8_t note;
    uint8_t velocity;
} midi_note_t;

typedef struct {
    uint8_t channel;
    uint8_t controller;
    uint8_t value;
} midi_cc_t;

typedef union {
    midi_note_t note;
    midi_cc_t cc;
} midi_data_t;

/**
 * @brief Represents a parsed MIDI message.
 *
 * This structure contains the type of the MIDI message and its
 * associated data. The union field varies depending on the type.
 */
typedef struct {
    midi_message_type_t type;
    midi_data_t data;
} midi_message_t;

/**
 * @brief Enumeration of the states of the FSM.
 */
typedef enum {
    MIDI_FSM_WAIT_STATUS,
    MIDI_FSM_WAIT_DATA1,
    MIDI_FSM_WAIT_DATA2,
} midi_parser_state_t;

typedef void (*midi_message_handler_t)(const midi_message_t *);

/**
 * @brief Converts a MIDI note number to a human-readable note name string.
 *
 * This function takes a MIDI note number in the range 0–127 and returns
 * a string representing the musical note name (e.g., "C4", "A#3", "F#5").
 * It uses the General MIDI convention where note 60 is "C4" and octave numbering starts at -1.
 *
 * @param note_number The MIDI note number (0–127).
 * @return A pointer to a static string containing the note name.
 *         The returned pointer should not be freed or modified.
 *
 * @note The return value points to a static buffer. If the function is
 *       called multiple times in a single expression, the result may be overwritten.
 */
const char *midi_note_name(uint8_t note_number);

/**
 * @brief Parses incoming MIDI bytes into structured MIDI messages.
 *
 * This function implements a stateful MIDI parser that processes one byte at a time.
 * It recognizes standard 3-byte MIDI messages (e.g., Note On, Note Off, Control Change)
 * and calls the provided handler function when a complete message is parsed.
 *
 * The parser maintains internal state to track status and data bytes. It does not currently
 * handle running status, SysEx messages, or real-time messages.
 *
 * @param byte The next MIDI byte to parse (status or data).
 * @param handler A callback function that will be called with a complete MIDI message
 *                once it has been successfully parsed. May be NULL if no callback is needed.
 *
 * @note This function should be called for each received MIDI byte in order.
 *       It uses a static internal state and is not reentrant.
 */
void midi_parse_message(uint8_t byte, midi_message_handler_t handler);

#endif // _MIDI_PARSER_H
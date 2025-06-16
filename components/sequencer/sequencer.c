#include <stdint.h>
#include "controller.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "midi.h"
#include "sequencer.h"
#include "sequencer_fsm.h"
#include "sequencer_fsm_internal.h"
#include "time.h"

ESP_EVENT_DEFINE_BASE(SEQUENCER_EVENT);

static const char *TAG = "sequencer";
static uint8_t step_index = 0;
static uint8_t tempo = 120; // bpm
static uint8_t gate = 80; // percent
static TaskHandle_t xHandle = NULL;
static esp_event_loop_handle_t sequencer_event_loop;

static sequencer_step_sequence_t sequence = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};

//--------------------------------------
//  Private functions
//--------------------------------------

static void sequencer_step_select(uint8_t pos)
{
    step_index = pos;

    esp_event_post_to(
        sequencer_event_loop,
        SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP_SELECT,
        &step_index,
        sizeof(step_index),
        portMAX_DELAY
    );
}

/**
 * @brief Advances the sequencer to the next step.
 *
 * This function increments the current step index of the sequencer.
 * If the step index reaches the end of the sequence, it wraps around to the beginning.
 *
 * @note
 * This function does not take any input parameters and does not return any value.
 * It modifies the global state of the sequencer.
 *
 * @sa sequencer_reset
 */
static void sequencer_step_forward()
{
    sequencer_step_select((step_index + 1) % STEP_SEQUENCE_LENGTH);
}

/**
 * @brief Resets the sequencer to its initial state.
 *
 * This function resets the sequencer by setting the step index to 0.
 *
 * @note
 * This function does not take any input parameters and does not return
 * any value. It modifies the global variable `step_index` directly.
 */
static void sequencer_reset()
{
    sequencer_step_select(0);
}

/**
 * Get the full length of a single step in milliseconds
 */
static time_ms_t sequencer_get_step_duration_ms()
{
    return 60000 / tempo;
}

/**
 * Play the current step
 *
 * Emit an event with note and gate time.
 */
static void sequencer_step_trig()
{
    const sequencer_step_trig_event_t event = {
        .note = sequence[step_index],
        .gate_time_ms = sequencer_get_step_duration_ms() * gate / 100,
    };

    esp_event_post_to(
        sequencer_event_loop,
        SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP_TRIG,
        &event,
        sizeof(event),
        portMAX_DELAY
    );
}

/**
 * @brief Task function to play a hard-coded step sequence.
 *
 * This function runs in its own FreeRTOS task and plays a predefined
 * sequence of musical notes (C4 to C5), emitting events for each step
 * at a tempo defined by the global `tempo` variable.
 *
 * @param pvParameters Unused task parameters.
 */
static void sequencer_play_task(void *pvParameters)
{
    sequencer_reset();

    while (1) {
        sequencer_step_trig();
        vTaskDelay(pdMS_TO_TICKS(sequencer_get_step_duration_ms()));
        sequencer_step_forward();
    }
}

/**
 * @brief Starts the sequencer playback.
 *
 * Creates a FreeRTOS task to play the step sequence if it's not already running.
 * Logs a message and does nothing if the sequencer is already playing.
 */
static void on_enter_play()
{
    if (xHandle != NULL) {
        ESP_LOGI(TAG, "Sequencer is already running");
        return;
    }

    ESP_LOGI(TAG, "Creating the player task");
    xTaskCreate(sequencer_play_task, "sequencer_play", 2048, NULL, 5, &xHandle);
}

/**
 * @brief Stops the sequencer playback.
 *
 * Deletes the FreeRTOS task running the sequencer, if any, and resets the task handle.
 */
static void on_exit_play()
{
    if (xHandle != NULL) {
        ESP_LOGI(TAG, "Deleting the player task");
        vTaskDelete(xHandle);

        xHandle = NULL;
    }
}

static void on_note_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (sequencer_fsm_get_state() == SEQUENCER_STATE_REC) {
        midi_note_t note = *(midi_note_t *) data;
        // ESP_LOGI(TAG, "Recorded note %s at %d", midi_note_name(note), step_index);
        sequence[step_index] = note;
        sequencer_step_forward();
    }
}

static void register_note_handler_task(void *pvParameters)
{
    esp_event_handler_register_with(sequencer_event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_NOTE, on_note_event, NULL);
    vTaskDelete(NULL);
}

static void unregister_note_handler_task(void *pvParameters)
{
    esp_event_handler_unregister_with(sequencer_event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_NOTE, on_note_event);
    vTaskDelete(NULL);
}

static void on_enter_record()
{
    sequencer_reset();
    // Workaround for the rule that a handler cannot be registered by another handler.
    xTaskCreate(register_note_handler_task, NULL, 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "Recording started");
}

static void on_exit_record()
{
    xTaskCreate(unregister_note_handler_task, NULL, 2048, NULL, 5, NULL);
}

//--------------------------------------
//  Public functions
//--------------------------------------

/**
 * @brief Initializes the sequencer
 *
 * Registers event handlers for the SEQUENCER_EVENT base, including
 * handlers for PLAY and STOP events.
 *
 * @param loop The event loop handle to register the handlers with.
 */
void sequencer_init(esp_event_loop_handle_t event_loop)
{
    sequencer_event_loop = event_loop;

    sequencer_fsm_init(event_loop);
    sequencer_fsm_set_hooks(SEQUENCER_STATE_PLAY, on_enter_play, on_exit_play);
    sequencer_fsm_set_hooks(SEQUENCER_STATE_REC, on_enter_record, on_exit_record);

    // Subscribe to the note key events
    esp_event_handler_register_with(event_loop, CONTROLLER_EVENT, CONTROLLER_EVENT_NOTE, on_note_event, NULL);
}
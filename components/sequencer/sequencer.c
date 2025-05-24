#include <stdint.h>
#include "controller.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "midi/notes.h"
#include "midi/parser.h"
#include "sequencer.h"

ESP_EVENT_DEFINE_BASE(SEQUENCER_EVENT);

typedef enum {
    SEQUENCER_STATE_STOP,
    SEQUENCER_STATE_PLAY,
    SEQUENCER_STATE_REC,
} sequencer_state_t;

static const char *TAG = "sequencer";
static uint8_t step_index = 0;
static uint8_t tempo = 120; // bpm
static TaskHandle_t xHandle = NULL;
static esp_event_loop_handle_t event_loop;
static sequencer_state_t current_state = SEQUENCER_STATE_STOP;

//--------------------------------------
//  Private functions
//--------------------------------------

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
    step_index = (step_index + 1) % STEP_SEQUENCE_LENGTH;
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
    step_index = 0;
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
    // StepSequence sequence = {
    //     {NOTE_C4},
    //     {NOTE_D4},
    //     {NOTE_E4},
    //     {NOTE_F4},
    //     {NOTE_G4},
    //     {NOTE_A4},
    //     {NOTE_B4},
    //     {NOTE_C5}
    // };

    uint32_t step_duration_ms = 60000 / tempo;

    sequencer_reset();

    while (1) {
        esp_event_post_to(
            event_loop,
            SEQUENCER_EVENT,
            SEQUENCER_EVENT_STEP_SELECT,
            &step_index,
            sizeof(step_index),
            portMAX_DELAY
        );

        vTaskDelay(pdMS_TO_TICKS(step_duration_ms));

        sequencer_step_forward();
    }
}

/**
 * @brief Starts the sequencer playback.
 *
 * Creates a FreeRTOS task to play the step sequence if it's not already running.
 * Logs a message and does nothing if the sequencer is already playing.
 */
static void sequencer_start_playing()
{
    current_state = SEQUENCER_STATE_PLAY;

    if (xHandle != NULL) {
        ESP_LOGI(TAG, "Sequencer is already running");
        return;
    }

    ESP_LOGI(TAG, "Creating the player task");
    xTaskCreate(sequencer_play_task, "sequencer_start", 2048, NULL, 5, &xHandle);
}

/**
 * @brief Stops the sequencer playback.
 *
 * Deletes the FreeRTOS task running the sequencer, if any, and resets the task handle.
 */
static void sequencer_stop_playing()
{
    current_state = SEQUENCER_STATE_STOP;

    if (xHandle != NULL) {
        ESP_LOGI(TAG, "Deleting the player task");
        vTaskDelete(xHandle);

        xHandle = NULL;
    }
}

static void sequencer_start_recording()
{
    current_state = SEQUENCER_STATE_REC;

    sequencer_reset();

    ESP_LOGI(TAG, "Recording started");
    // esp_event_post_to(
    //     event_loop,
    //     SEQUENCER_EVENT,
    //     SEQUENCER_EVENT_STEP_SELECT,
    //     &step_index,
    //     sizeof(step_index),
    //     portMAX_DELAY
    // );
}

static void sequencer_stop_recording()
{
    current_state = SEQUENCER_STATE_STOP;
}

static void sequencer_record_note(uint32_t note)
{
    ESP_LOGI(TAG, "Recorded note %s at %d", midi_note_name(note), step_index);
    sequencer_step_forward();
}

/**
 * @brief Listener for the CONTROLLER_EVENT based events.
 *
 * @param handler_arg Unused.
 * @param base Event base.
 * @param id Event ID.
 * @param event_data Event-specific data (unused).
 */
static void controller_event_handler(
    void* handler_arg,
    esp_event_base_t base,
    int32_t id,
    void* event_data
) {
    switch (id) {
        case CONTROLLER_EVENT_PLAY:
            if (current_state == SEQUENCER_STATE_STOP) {
                sequencer_start_playing();
            }
            break;

        case CONTROLLER_EVENT_STOP:
            switch (current_state) {
                case SEQUENCER_STATE_PLAY:
                    sequencer_stop_playing();
                    break;

                case SEQUENCER_STATE_REC:
                    sequencer_stop_recording();
                    break;

                default:
                    break;
            }
            break;

        case CONTROLLER_EVENT_REC:
            if (current_state == SEQUENCER_STATE_STOP) {
                sequencer_start_recording();
            }
            break;

        case CONTROLLER_EVENT_NOTE:
            if (current_state == SEQUENCER_STATE_REC) {
                uint8_t note = *(uint8_t*) event_data;
                sequencer_record_note(note);
            }
            break;

        default:
            break;
    }
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
void sequencer_init(esp_event_loop_handle_t loop)
{
    event_loop = loop;

    esp_event_handler_register_with(
        event_loop,
        CONTROLLER_EVENT,
        ESP_EVENT_ANY_ID,
        controller_event_handler,
        NULL
    );
}
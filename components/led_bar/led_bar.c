#include <stdint.h>
#include "driver/gpio.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_bar.h"
#include "sequencer.h"
#include "tpic6b595.h"

#define BLINK_DELAY_MS 250  // 2Hz = 500ms cycle

static TaskHandle_t xHandle = NULL;
static tpic6b595_t shift_reg = {
    .srck = GPIO_NUM_33,
    .rck  = GPIO_NUM_32,
    .ser  = GPIO_NUM_23,
    .oe   = GPIO_NUM_22
};

static void led_bar_set_active_only(uint8_t pos)
{
    tpic6b595_write(&shift_reg, 0x80 >> pos);
}

/**
 * @brief Sequencer listener function for handling sequencer step events.
 *
 * This function is called when a sequencer step event is triggered.
 * It sets the active LED bar to the index of the current step.
 *
 * @param handler_arg User-provided argument passed to the event handler registration.
 * @param base The event base associated with the event.
 * @param id The event ID, which is not used in this function.
 * @param event_data A pointer to the event data, which is a sequencer_step_event_t structure.
 */
static void sequencer_listener(
    void* handler_arg,
    esp_event_base_t base,
    int32_t id,
    void* event_data
) {
    uint8_t step_index = *(uint8_t*) event_data;
    led_bar_set_active_only(step_index);
}

static void led_bar_blink_task(void *pvParameters)
{
    while (1) {
        // Because output is enabled by default, we start the blink
        // with turning the LEDs off.
        tpic6b595_output_disable(&shift_reg);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));

        tpic6b595_output_enable(&shift_reg);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

static void led_bar_start_blinking()
{
    if (xHandle == NULL) {
        xTaskCreate(led_bar_blink_task, "led_bar_blink", 2048, NULL, 5, &xHandle);
    }
}

static void led_bar_stop_blinking()
{
    if (xHandle != NULL) {
        vTaskDelete(xHandle);
        xHandle = NULL;
    }

    // Left the output enabled after the blinking
    tpic6b595_output_enable(&shift_reg);
}

/**
 * @brief Initializes the LED bar using TPIC6B595 shift register.
 *
 * This function initializes the LED bar by setting up the TPIC6B595 shift register.
 * It also registers an event handler for sequencer events.
 *
 * @param event_loop The event loop handle to register the event handler with.
 */
void led_bar_init(esp_event_loop_handle_t event_loop)
{
    if (tpic6b595_init(&shift_reg) != ESP_OK) {
        printf("Failed to init TPIC6B595\n");
        return;
    }

    // Turn off all LEDs
    tpic6b595_write(&shift_reg, 0);

    esp_event_handler_register_with(
        event_loop,
        SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP_SELECT,
        sequencer_listener,
        NULL
    );
}
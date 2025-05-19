#include <stdint.h>
#include "driver/gpio.h"
#include "esp_event.h"
#include "led_bar.h"
#include "sequencer.h"
#include "tpic6b595.h"

static tpic6b595_t shift_reg = {
    .srck = GPIO_NUM_33,
    .rck  = GPIO_NUM_32,
    .ser  = GPIO_NUM_22
};

static void led_bar_set_active_only(uint8_t pos)
{
    tpic6b595_write(&shift_reg, 0x80 >> pos);
}

static void sequencer_listener(
    void* handler_arg,
    esp_event_base_t base,
    int32_t id, void* event_data
) {
    sequencer_step_event_t* ev = (sequencer_step_event_t*) event_data;
    led_bar_set_active_only(ev->step_index);
}

void led_bar_init(esp_event_loop_handle_t event_loop)
{
    if (tpic6b595_init(&shift_reg) != ESP_OK) {
        printf("Failed to init TPIC6B595\n");
        return;
    }

    // Turn off all LEDs
    tpic6b595_write(&shift_reg, 0);

    esp_event_handler_register_with(event_loop, SEQUENCER_EVENT,
        SEQUENCER_EVENT_STEP,
        sequencer_listener, NULL);
}
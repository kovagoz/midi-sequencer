#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tpic6b595.h"

void app_main(void)
{
    tpic6b595_t shift_reg = {
        .srck = GPIO_NUM_33,
        .rck  = GPIO_NUM_32,
        .ser  = GPIO_NUM_22
    };

    if (tpic6b595_init(&shift_reg) != ESP_OK) {
        printf("Failed to init TPIC6B595\n");
        return;
    }

    while (1) {
        for (int i = 7; i >= 0; i--) {
            tpic6b595_write(&shift_reg, 1 << i);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

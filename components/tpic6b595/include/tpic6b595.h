#pragma once

#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t srck;  // Shift register clock (SRCK)
    gpio_num_t rck;   // Latch clock (RCK)
    gpio_num_t ser;   // Serial input (SER)
} tpic6b595_t;

/**
 * @brief Initialize the TPIC6B595 driver with specified GPIOs
 *
 * @param dev Pointer to a tpic6b595_t struct with pin definitions
 * @return esp_err_t ESP_OK on success, error otherwise
 */
esp_err_t tpic6b595_init(const tpic6b595_t *dev);

/**
 * @brief Shift out one byte to the shift register
 *
 * @param dev Pointer to initialized tpic6b595_t
 * @param data 8-bit data to send (bit 7 = DRAIN7, ..., bit 0 = DRAIN0)
 */
void tpic6b595_write(const tpic6b595_t *dev, uint8_t data);
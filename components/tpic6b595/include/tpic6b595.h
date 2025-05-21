#pragma once

#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t srck;  // Shift register clock (SRCK)
    gpio_num_t rck;   // Latch clock (RCK)
    gpio_num_t ser;   // Serial input (SER)
    gpio_num_t oe;    // Output enable (G)
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

/**
 * @brief Enables the output of the TPIC6B595 shift register.
 *
 * This function sets the output enable (OE) pin of the TPIC6B595 shift register to low,
 * enabling the output of the shift register.
 *
 * @param dev A pointer to a tpic6b595_t structure representing the TPIC6B595 device.
 * This structure should contain the GPIO pin number for the output enable pin.
 *
 * @note This function does not return a value. It directly modifies the state of the
 * OE pin on the TPIC6B595 device.
 */
void tpic6b595_output_enable(const tpic6b595_t *dev);

/**
 * @brief Disables the output of the TPIC6B595 shift register.
 *
 * This function disables the output of the TPIC6B595 shift register by setting the Output Enable (OE) pin to a high level.
 *
 * @param dev A pointer to the TPIC6B595 device structure.
 *
 * The TPIC6B595 device structure should contain the following members:
 * - `gpio_num oe`: The GPIO number connected to the Output Enable (OE) pin of the TPIC6B595.
 *
 * @note This function does not return a value. It directly modifies the state of the OE pin.
 */
void tpic6b595_output_disable(const tpic6b595_t *dev);
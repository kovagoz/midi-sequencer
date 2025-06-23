#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Initializes the sequencer timer and prepares it for use.
 *
 * This function sets up the GPTimer with a fixed resolution, registers the ISR,
 * and binds it to a pointer to the task handle that will be notified on each timer event.
 *
 * @param task_handle Pointer to a TaskHandle_t that will receive notifications on timer alarms.
 *        The task itself can be created after this call, but the handle must be valid before the timer fires.
 */
void sequencer_timer_init(TaskHandle_t *task_handle);

/**
 * @brief Sets the step duration for the sequencer timer in milliseconds.
 *
 * This function updates the timer's alarm period. If the timer is already running,
 * the new value will apply to the next interval.
 *
 * @param step_duration_ms Duration of each sequencer step, in milliseconds.
 */
void sequencer_timer_set_period(uint32_t step_duration_ms);

/**
 * @brief Starts the sequencer timer with the given step duration.
 *
 * This enables periodic interrupts at the specified interval.
 * The associated task will receive a notification on each step.
 *
 * @param step_duration_ms Duration of each sequencer step, in milliseconds.
 */
void sequencer_timer_start(uint32_t step_duration_ms);

/**
 * @brief Stops the sequencer timer.
 *
 * Disables the timer without deinitializing it. Call this when playback is stopped.
 */
void sequencer_timer_stop(void);
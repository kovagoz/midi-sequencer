#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void metronome_init();
void metronome_notify(TaskHandle_t *task_handle);
uint32_t metronome_get_period_ms();
void metronome_set_tempo(uint8_t bpm);
void metronome_incr_tempo();
void metronome_decr_tempo();
void metronome_start();
void metronome_stop();
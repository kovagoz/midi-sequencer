#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "internal/play.h"
#include "internal/core.h"
#include "internal/tempo.h"
#include "metronome.h"

static TaskHandle_t player_task_handle = NULL;

static void play_task(void *pvParameters)
{
    sequencer_step_reset();

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for timer ISR
        sequencer_step_trig();
        sequencer_step_forward();
    }
}

static void delete_play_task()
{
    vTaskDelete(player_task_handle);
    player_task_handle = NULL;
}

void sequencer_on_enter_play()
{
    if (player_task_handle != NULL) {
        return;
    }

    xTaskCreate(play_task, NULL, 2048, NULL, 5, &player_task_handle);

    metronome_notify(&player_task_handle);
    metronome_start();

    sequencer_bpm_control_enable();
}

void sequencer_on_exit_play()
{
    if (player_task_handle != NULL) {
        metronome_stop();
        delete_play_task();
        sequencer_bpm_control_disable();
    }
}
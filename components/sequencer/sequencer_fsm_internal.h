#pragma once

#include "esp_event.h"

typedef void (*state_hook_t)(void);

void sequencer_fsm_init(esp_event_loop_handle_t event_loop);
sequencer_state_t sequencer_fsm_get_state(void);
void sequencer_fsm_set_hooks(sequencer_state_t state, state_hook_t on_enter, state_hook_t on_exit);
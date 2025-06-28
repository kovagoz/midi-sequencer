#include <stdint.h>
#include "esp_event.h"
#include "internal/event.h"
#include "internal/fsm.h"
#include "internal/persist.h"
#include "internal/play.h"
#include "internal/record.h"
#include "internal/core.h"
#include "metronome.h"
#include "sequencer.h"
#include "sequencer_fsm.h"

ESP_EVENT_DEFINE_BASE(SEQUENCER_EVENT);

void sequencer_init(esp_event_loop_handle_t event_loop)
{
    sequencer_set_event_loop(event_loop);
    sequencer_load_sequence();

    metronome_init();

    sequencer_fsm_init(event_loop);

    sequencer_fsm_set_hooks(
        SEQUENCER_STATE_PLAY,
        sequencer_on_enter_play,
        sequencer_on_exit_play
    );

    sequencer_fsm_set_hooks(
        SEQUENCER_STATE_REC,
        sequencer_on_enter_record,
        sequencer_on_exit_record
    );
}
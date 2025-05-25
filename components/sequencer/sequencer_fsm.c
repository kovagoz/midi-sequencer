#include "controller.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sequencer.h"
#include "sequencer_fsm.h"
#include "sequencer_fsm_internal.h"

static const char *TAG = "sequencer_fsm";

typedef struct {
    sequencer_state_t current;
    controller_event_t event;
    sequencer_state_t next;
} fsm_transition_t;

typedef struct {
    state_hook_t on_enter;
    state_hook_t on_exit;
} state_hooks_t;

static sequencer_state_t current_state = SEQUENCER_STATE_STOP;
static esp_event_loop_handle_t fsm_event_loop;
static state_hooks_t state_hooks[SEQUENCER_STATE_MAX]; // One for each sequencer_state_t

static const fsm_transition_t fsm_table[] = {
    { SEQUENCER_STATE_STOP, CONTROLLER_EVENT_PLAY, SEQUENCER_STATE_PLAY },
    { SEQUENCER_STATE_STOP, CONTROLLER_EVENT_REC,  SEQUENCER_STATE_REC  },
    { SEQUENCER_STATE_PLAY, CONTROLLER_EVENT_STOP, SEQUENCER_STATE_STOP },
    { SEQUENCER_STATE_REC,  CONTROLLER_EVENT_STOP, SEQUENCER_STATE_STOP },
};

ESP_EVENT_DEFINE_BASE(SEQUENCER_STATE_CHANGE_EVENT_BASE);

static void sequencer_fsm_broadcast_state_change(
    sequencer_state_t previous_state,
    sequencer_state_t current_state
) {
    sequencer_state_change_event_t ev = {
        .previous = previous_state,
        .current  = current_state
    };

    esp_event_post_to(
        fsm_event_loop,
        SEQUENCER_STATE_CHANGE_EVENT_BASE,
        current_state,  // Event ID is the new state
        &ev,
        sizeof(ev),
        portMAX_DELAY
    );
}

static void sequencer_fsm_call_exit_hook()
{
    if (state_hooks[current_state].on_exit) {
        state_hooks[current_state].on_exit();
    }
}

static void sequencer_fsm_call_enter_hook()
{
    if (state_hooks[current_state].on_enter) {
        state_hooks[current_state].on_enter();
    }
}

static size_t sequencer_fsm_table_size()
{
    return sizeof(fsm_table) / sizeof(fsm_table[0]);
}

static void sequencer_fsm_event_handler(
    void *handler_args,
    esp_event_base_t base,
    int32_t id,
    void *event_data
) {
    controller_event_t event = (controller_event_t) id;

    for (size_t i = 0; i < sequencer_fsm_table_size(); ++i) {
        if (fsm_table[i].current == current_state && fsm_table[i].event == event) {
            sequencer_fsm_call_exit_hook();

            // Change state
            sequencer_state_t previous = current_state;
            sequencer_state_t next = fsm_table[i].next;
            ESP_LOGI(TAG, "FSM transition: %d -> %d", previous, next);
            current_state = next;

            sequencer_fsm_call_enter_hook();
            sequencer_fsm_broadcast_state_change(previous, current_state);

            return;
        }
    }

    // ESP_LOGW(TAG, "Invalid transition from %d on event %d", current_state, event);
}

void sequencer_fsm_init(esp_event_loop_handle_t event_loop)
{
    fsm_event_loop = event_loop;
    current_state  = SEQUENCER_STATE_STOP;

    ESP_ERROR_CHECK(esp_event_handler_register_with(
        fsm_event_loop,
        CONTROLLER_EVENT,
        ESP_EVENT_ANY_ID,
        sequencer_fsm_event_handler,
        NULL
    ));

    ESP_LOGI(TAG, "FSM initialized in STOP state");
}

void sequencer_fsm_set_hooks(sequencer_state_t state, state_hook_t on_enter, state_hook_t on_exit)
{
    if (state < 0 || state >= SEQUENCER_STATE_MAX) {
        return;
    }

    state_hooks[state].on_enter = on_enter;
    state_hooks[state].on_exit  = on_exit;
}

sequencer_state_t sequencer_fsm_get_state(void)
{
    return current_state;
}
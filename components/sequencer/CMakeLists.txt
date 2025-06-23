set(COMPONENT_REQUIRES "midi" "esp_event" "controller" "common" "storage" "driver")

set(COMPONENT_SRCS "sequencer.c" "sequencer_fsm.c" "sequencer_timer.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
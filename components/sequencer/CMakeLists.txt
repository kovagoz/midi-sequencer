set(COMPONENT_REQUIRES "midi" "esp_event" "controller" "common" "storage")

set(COMPONENT_SRCS "sequencer.c" "sequencer_fsm.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
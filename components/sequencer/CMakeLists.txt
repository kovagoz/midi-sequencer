set(COMPONENT_REQUIRES "midi" "esp_event" "controller" "common" "storage" "metronome")

set(COMPONENT_SRCS "sequencer.c" "sequencer_fsm.c" "event.c" "persist.c" "play.c" "record.c" "core.c" "tempo.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
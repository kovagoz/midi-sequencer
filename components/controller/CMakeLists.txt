set(COMPONENT_REQUIRES "esp_event" "esp_driver_gpio" "esp_driver_uart" "midi" "sequencer")

set(COMPONENT_SRCS "controller.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
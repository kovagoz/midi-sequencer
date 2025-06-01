set(COMPONENT_REQUIRES "midi" "esp_event" "sequencer" "esp_driver_gpio" "esp_driver_uart")

set(COMPONENT_SRCS "transmitter.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
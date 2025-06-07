set(COMPONENT_REQUIRES "esp_driver_uart" "esp_driver_gpio")

set(COMPONENT_SRCS "parser.c" "input.c" "output.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
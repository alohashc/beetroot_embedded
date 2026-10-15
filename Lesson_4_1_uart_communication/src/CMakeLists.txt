idf_component_register(
    SRCS "esp32/main.c"
    INCLUDE_DIRS "../include"
    REQUIRES esp_driver_uart
)
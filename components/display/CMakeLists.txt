idf_component_register(
    SRCS "touch_driver.c" "display_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES driver lvgl esp_lcd esp_lcd_touch
)
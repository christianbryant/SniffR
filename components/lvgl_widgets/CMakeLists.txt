set(srcs "lvgl_arc.c"
         "lvgl_home_page.c")

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include"
    REQUIRES driver lvgl esp_lcd esp_lcd_touch esp_adc
)
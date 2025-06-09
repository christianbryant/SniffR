set(srcs "esp_lcd_st7796.c"
         "esp_lcd_st7796_general.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "priv_include"
                       REQUIRES "esp_lcd"
                       PRIV_REQUIRES "driver")

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H
#include "esp_err.h"




void lv_example_get_started_1(void);
esp_err_t display_hw_init(void);
esp_err_t display_set_brightness(int percent);

#endif // SCD40_H
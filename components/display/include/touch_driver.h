#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"
#include "touch_driver.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_ft5x06.h"

void lvgl_touchpad_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
esp_err_t touch_i2c_init(i2c_port_t i2c_num, i2c_master_dev_handle_t *i2c_dev, gpio_num_t sda_pin, gpio_num_t scl_pin);
esp_err_t touch_hw_init(i2c_master_bus_handle_t tp_bus_handle);

#endif // TOUCH_DRIVER_H
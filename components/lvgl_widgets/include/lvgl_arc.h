#ifndef LVLG_ARCH_H
#define LVLG_ARCH_H
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"

void update_arc_color(lv_obj_t *arc, lv_color_t color);

void update_arc_values(lv_timer_t *timer);
void rand_update_co2_arc_value(lv_obj_t *arc, int32_t co2_value);
void rand_update_temp_arc_value(lv_obj_t *arc, float temp_value);
void rand_update_humid_arc_value(lv_obj_t *arc, float hum_value);

lv_obj_t *create_dynamic_co2_arc(lv_obj_t *parent);
lv_obj_t *create_dynamic_temp_arc(lv_obj_t *parent);
lv_obj_t *create_dynamic_humid_arc(lv_obj_t *parent);

void create_arcs(lv_obj_t *parent, i2c_master_dev_handle_t i2c_handle);

#endif // LVLG_ARCH_H

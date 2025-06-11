#ifndef LVGL_HOME_PAGE_H
#define LVGL_HOME_PAGE_H
#include "lvgl.h"
#include "lvgl_arc.h"
#include "scd40.h"

static void settings_btn_event_handler(lv_event_t *e);

void top_bar_create(lv_obj_t *parent);

void update_battery_icon(lv_obj_t *battery_icon, int battery_level);

void create_home_page(i2c_master_dev_handle_t i2c_handle);

#endif // LVGL_HOME_PAGE_H
#ifndef LVGL_HOME_PAGE_H
#define LVGL_HOME_PAGE_H
#include "lvgl.h"
#include "lvgl_arc.h"
#include "scd40.h"

static void settings_btn_event_handler(lv_event_t *e);

void top_bar_create(lv_obj_t *parent);

void create_home_page();

extern lv_timer_t *bat_timer;

#endif // LVGL_HOME_PAGE_H

#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"

void update_arc_color(lv_obj_t *arc, lv_color_t color);

void rand_update_co2_arc_value(lv_timer_t *timer);
void create_dynamic_co2_arc(lv_obj_t *parent);

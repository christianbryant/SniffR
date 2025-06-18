#ifndef LVLG_ARCH_H
#define LVLG_ARCH_H
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"

typedef struct {
    float min, max;
    lv_color_t color;
} arc_threshold_t;

typedef struct {
    lv_obj_t *arc_obj;
    float value;
    const char* label_type;
    const char *label_format;
    arc_threshold_t *thresholds;
    size_t threshold_count;
    float min,max;
} arc_config_t;

typedef struct {
    arc_config_t *co2_arc_config;
    arc_config_t *temp_arc_config;
    arc_config_t *humid_arc_config;
} lv_timer_data;

void update_arc_color(lv_obj_t *arc, lv_color_t color);

void update_all_arcs(lv_timer_t *timer);
void update_arc_value(const arc_config_t *config);
// void rand_update_co2_arc_value(lv_obj_t *arc, int32_t co2_value);
// void rand_update_temp_arc_value(lv_obj_t *arc, float temp_value);
// void rand_update_humid_arc_value(lv_obj_t *arc, float hum_value);

// lv_obj_t *create_dynamic_co2_arc(lv_obj_t *parent);
// lv_obj_t *create_dynamic_temp_arc(lv_obj_t *parent);
// lv_obj_t *create_dynamic_humid_arc(lv_obj_t *parent);
void create_dynamic_arc(lv_obj_t *parent, arc_config_t *config);

void create_arcs(lv_obj_t *parent);

#endif // LVLG_ARCH_H

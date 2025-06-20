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

extern lv_timer_t *arc_timer;

void update_arc_color(lv_obj_t *arc, lv_color_t color);

void update_all_arcs(lv_timer_t *timer);
void update_arc_value(const arc_config_t *config);
void create_dynamic_arc(lv_obj_t *parent, arc_config_t *config);

void create_arcs(lv_obj_t *parent);

#endif // LVLG_ARCH_H

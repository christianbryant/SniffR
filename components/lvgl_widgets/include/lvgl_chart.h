#ifndef LVGL_CHART_H
#define LVGL_CHART_H
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>

static lv_obj_t * chart;
static lv_chart_series_t * series;
static int time_in_seconds = 0;
static uint32_t local_max = 0;
static lv_obj_t * x_axis_container;

static void update_x_axis_labels(lv_obj_t * chart, lv_obj_t * label_container, uint32_t time_in_seconds, uint32_t point_count);

static void chart_update_cb(lv_timer_t * timer);

void create_dynamic_chart(void);

#endif // LVGL_CHART_H
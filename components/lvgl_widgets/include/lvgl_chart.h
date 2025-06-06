#include "lvgl.h"
#include <stdlib.h>
#include <string.h>

static lv_obj_t * chart;
static lv_chart_series_t * series;
static int time_in_seconds = 0;
static uint32_t local_max = 0;
static lv_obj_t * x_axis_container;

static void update_x_axis_labels(lv_obj_t * chart, lv_obj_t * label_container, uint32_t time_in_seconds, uint32_t point_count) {
    // Clear existing labels
    lv_obj_clean(label_container);

    // Get chart width
    int chart_width = lv_obj_get_width(chart);
    int x_step = chart_width / point_count;

    for (uint32_t i = 0; i < point_count; i += 5) {
        char buf[16];
        int seconds_ago = time_in_seconds - (point_count - 1 - i) * 5;
        snprintf(buf, sizeof(buf), "%" PRIu32, seconds_ago);

        lv_obj_t * lbl = lv_label_create(label_container);
        lv_label_set_text(lbl, buf);

        // Optionally style the label
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);

        lv_obj_set_size(lbl, x_step, 20);  // width matches one point segment
    }
}

static void chart_update_cb(lv_timer_t * timer)
{
    uint16_t ppm_value = rand() % 5000;

    if (ppm_value > local_max) {
        local_max = ppm_value;
    }

    uint16_t y_max = local_max < 5000 ? local_max : 5000;
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, y_max);

    lv_chart_set_next_value(chart, series, ppm_value);

    uint16_t current_points = lv_chart_get_point_count(chart);
    lv_chart_set_point_count(chart, current_points + 1);

    lv_chart_refresh(chart);

    // Update X axis labels
    update_x_axis_labels(chart, x_axis_container, time_in_seconds, current_points + 1);

    time_in_seconds += 5;
}

void create_dynamic_chart(void)
{
    chart = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart, 300, 200);
    lv_obj_center(chart);
    
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);  // Connect points with lines
    lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart, 4, 4, LV_PART_INDICATOR);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
    lv_chart_set_point_count(chart, 20);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

    lv_obj_set_scroll_dir(chart, LV_DIR_LEFT);
    lv_obj_set_scroll_snap_x(chart, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(chart, LV_SCROLLBAR_MODE_AUTO);

    series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

    for (int i = 0; i < lv_chart_get_point_count(chart); i++) {
        lv_chart_set_next_value(chart, series, 0);
    }

    // Create container for X-axis labels
    x_axis_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(x_axis_container, 300, 20);  // Width matches chart width
    lv_obj_align_to(x_axis_container, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);  // Below the chart
    lv_obj_set_flex_flow(x_axis_container, LV_FLEX_FLOW_ROW);  // horizontal layout
    lv_obj_set_scroll_dir(x_axis_container, LV_DIR_HOR);
    lv_obj_set_style_pad_row(x_axis_container, 0, 0);
    lv_obj_set_style_pad_column(x_axis_container, 0, 0);

    lv_timer_create(chart_update_cb, 5000, NULL);
}


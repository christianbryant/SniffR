#include "lvgl.h"
#include "lv_color.h"
#include "esp_log.h"
#include "esp_err.h"

#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"

#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"

#include <inttypes.h>

esp_err_t lvgl_widget_chart(void){
    // Create a chart widget
    lv_obj_t *chart = lv_chart_create(lvgl_port_get_disp());
    lv_obj_set_size(chart, 240, 120);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);

    // Set the type of the chart
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    // Add a series to the chart
    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
    if (ser == NULL) {
        ESP_LOGE("LVGL", "Failed to add series to chart");
        return ESP_FAIL;
    }
    // Set the range of the chart
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 100);
    // Set the number of points in the series


    // Add data points to the series
    for (int i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser, i * 10);
    }
    lv_chart_refresh(chart);
    ESP_LOGI("LVGL", "Chart widget created successfully");

    return ESP_OK;
}
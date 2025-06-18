
#include "lvgl.h"
#include "lvgl_arc.h"
#include "scd40.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "i2c_driver.h"
#include "nvs_driver.h"
#include "float.h"

static const char* TAG = "lvgl_arc";

void update_arc_color(lv_obj_t *arc, lv_color_t color)
{
    // Update the arc's color
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
}

void update_all_arcs(lv_timer_t *timer){
    lv_timer_data *data = (lv_timer_data *) lv_timer_get_user_data(timer);
    arc_config_t *co2_arc = data->co2_arc_config;
    arc_config_t *temp_arc = data->temp_arc_config;
    arc_config_t *humid_arc = data->humid_arc_config;
    uint16_t co2_value = 0;
    float temp_value = 0.0f;
    float humid_value = 0.0f;
    esp_err_t err = scd40_read_measurement(i2c_scd40, &co2_value, &temp_value, &humid_value);
    while (err != ESP_OK) {
        ESP_LOGE("LVGL", "Failed to read SCD40 measurement: %s", esp_err_to_name(err));
        err = scd40_read_measurement(i2c_scd40, &co2_value, &temp_value, &humid_value);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before retrying
    }

    co2_arc->value = co2_value;
    temp_arc->value = temp_value;
    humid_arc->value = humid_value;
    
    update_arc_value(co2_arc);
    update_arc_value(temp_arc);
    update_arc_value(humid_arc);
}

void update_arc_value(const arc_config_t *config)
{
    lv_arc_set_value(config->arc_obj, config->value);

    // Pick color based on threshold ranges
    for (int i = 0; i < config->threshold_count; ++i) {
        arc_threshold_t t = config->thresholds[i];
        if (config->value >= t.min && config->value <= t.max) {
            update_arc_color(config->arc_obj, t.color);
            break;
        }
    }

    // Update label text
    lv_obj_t *cont = lv_obj_get_child(config->arc_obj, 0);
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        if (num_label) {
            char buf[16];
            int ret = snprintf(buf, sizeof(buf), config->label_format, (int)(config->value));
            if(ret > 16){
                ESP_LOGE(TAG, "Buffer not large enough for expected format");
            } else if(ret < 0){
                ESP_LOGE(TAG, "Error returned from snprintf()");
            }
            lv_label_set_text(num_label, buf);
        }
    }
}

void create_dynamic_arc(lv_obj_t *parent, arc_config_t *config){

    lv_obj_t *cont1 = lv_obj_create(parent);
    lv_obj_set_size(cont1, 130, 130);
    lv_obj_set_align(cont1, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(cont1, lv_color_hex(0x11273C), LV_PART_MAIN);
    // Crashes the display if set to transparent??
    // lv_obj_set_style_bg_opa(cont1, LV_OPA_TRANSP, LV_PART_MAIN);  // Make transparent
    lv_obj_set_style_border_width(cont1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont1, 0, LV_PART_MAIN);

    lv_obj_t *arc = lv_arc_create(cont1);
    lv_obj_set_size(arc, 100, 100);
    lv_arc_set_bg_angles(arc, 135, 405);
    lv_obj_center(arc);
    
    // set the arcs color
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    
    // Set the arc's value and range
    lv_arc_set_value(arc, 0); // Set the current value
    
    lv_arc_set_range(arc, config->min, config->max); // Set the range of the arc
    
    // Set the arc's style
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);

    lv_obj_t *cont2 = lv_obj_create(arc);
    lv_obj_set_size(cont2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont2);
    lv_obj_set_style_bg_opa(cont2, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont2, 0, LV_PART_MAIN);
    // Use Flex layout
    lv_obj_set_layout(cont2, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create the number label
    lv_obj_t *num_label = lv_label_create(cont2);
    lv_obj_set_style_text_color(num_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(num_label, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_label_set_text_fmt(num_label, config->label_format, lv_arc_get_value(arc));

    // Create the Type label
    lv_obj_t *unit_label = lv_label_create(cont2);
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(unit_label, config->label_type);

    lv_obj_align(num_label, LV_ALIGN_CENTER, 0, -10); // Number label
    lv_obj_align(unit_label, LV_ALIGN_CENTER, 0, 20); // Type label


    config->arc_obj = arc;
}

void create_arcs(lv_obj_t *parent)
{   

    //Configure the arcs
    arc_threshold_t co2_ranges[] = {
        { 0, 999, lv_color_hex(0x00ff00) },
        { 1000, 1199, lv_color_hex(0xffff00) },
        { 1200, 1499, lv_color_hex(0xff8000) },
        { 1500, FLT_MAX, lv_color_hex(0xff0000) }
    };
    int co2_thresh_count = sizeof(co2_ranges) / sizeof(co2_ranges[0]);
    arc_threshold_t *co2_ranges_malloc = malloc(co2_thresh_count * sizeof(arc_threshold_t));
    memcpy(co2_ranges_malloc, co2_ranges, co2_thresh_count * sizeof(arc_threshold_t));

    arc_config_t *co2_arc_config = malloc(sizeof(arc_config_t));
    *co2_arc_config = (arc_config_t){
        .arc_obj = NULL,
        .value = 0.0,
        .label_type = "PPM",
        .label_format = "%ld",
        .thresholds = co2_ranges_malloc,
        .threshold_count = co2_thresh_count,
        .min = 0,
        .max = 2500,
    };

    arc_threshold_t temp_ranges[] = {
        { -10, 10, lv_color_hex(0x0000FF) },
        { 11, 20, lv_color_hex(0xffff00) },
        { 21, 30, lv_color_hex(0xff8000) },
        { 31, FLT_MAX, lv_color_hex(0xff0000) }
    };
    int temp_thresh_count = sizeof(temp_ranges) / sizeof(temp_ranges[0]);
    arc_threshold_t *temp_ranges_malloc = malloc(temp_thresh_count * sizeof(arc_threshold_t));
    memcpy(temp_ranges_malloc, temp_ranges, temp_thresh_count * sizeof(arc_threshold_t));

    arc_config_t *temp_arc_config = malloc(sizeof(arc_config_t));
    *temp_arc_config = (arc_config_t){
        .arc_obj = NULL,
        .value = 0.0,
        .label_type = "TEMP",
        .label_format = "%d°",
        .thresholds = temp_ranges_malloc,
        .threshold_count = temp_thresh_count,
        .min = -10,
        .max = 50,
    };

    arc_threshold_t humid_ranges[] = {
        { 0, 39, lv_color_hex(0xffff00) },
        { 40, 59, lv_color_hex(0xffff00) },
        { 90, FLT_MAX, lv_color_hex(0xff0000) }
    };
    int humid_thresh_count = sizeof(humid_ranges) / sizeof(humid_ranges[0]);
    arc_threshold_t *humid_ranges_malloc = malloc(humid_thresh_count * sizeof(arc_threshold_t));
    memcpy(humid_ranges_malloc, humid_ranges, humid_thresh_count * sizeof(arc_threshold_t));

    arc_config_t *humid_arc_config = malloc(sizeof(arc_config_t));
    *humid_arc_config = (arc_config_t){
        .arc_obj = NULL,
        .value = 0.0,
        .label_type = "HUMID",
        .label_format = "%d%%",
        .thresholds = humid_ranges_malloc,
        .threshold_count = humid_thresh_count,
        .min = 0,
        .max = 100,
    };

    // Create the arcs
    create_dynamic_arc(parent, co2_arc_config);
    create_dynamic_arc(parent, temp_arc_config);
    create_dynamic_arc(parent, humid_arc_config);

    // Create a timer to update the arcs periodically
    lv_timer_data *data = malloc(sizeof(lv_timer_data));
    data->co2_arc_config = co2_arc_config;
    data->temp_arc_config = temp_arc_config;
    data->humid_arc_config = humid_arc_config;

    lv_timer_t *arc_timer = lv_timer_create(update_all_arcs, 5000, data); // Update every second
    //lv_timer_ready(arc_timer);
}


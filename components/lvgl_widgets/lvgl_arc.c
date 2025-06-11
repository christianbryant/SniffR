
#include "lvgl.h"
#include "lvgl_arc.h"
#include "scd40.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_check.h"

struct lv_timer_data {
    i2c_master_dev_handle_t i2c_handle;
    lv_obj_t *co2_arc;
    lv_obj_t *temp_arc;
    lv_obj_t *humid_arc;
};

void update_arc_color(lv_obj_t *arc, lv_color_t color)
{
    // Update the arc's color
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
}

void update_arc_values(lv_timer_t *timer){
    struct lv_timer_data *data = (struct lv_timer_data *) lv_timer_get_user_data(timer);
    lv_obj_t *co2_arc = data->co2_arc;
    lv_obj_t *temp_arc = data->temp_arc;
    lv_obj_t *humid_arc = data->humid_arc;
    i2c_master_dev_handle_t i2c_handle = data->i2c_handle;
    uint16_t co2_value = 0;
    float temp_value = 0.0f;
    float hum_value = 0.0f;
    esp_err_t err = scd40_read_measurement(i2c_handle, &co2_value, &temp_value, &hum_value);
    while (err != ESP_OK) {
        ESP_LOGE("LVGL", "Failed to read SCD40 measurement: %s", esp_err_to_name(err));
        err = scd40_read_measurement(i2c_handle, &co2_value, &temp_value, &hum_value);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before retrying
    }
    
    // Update CO2 arc value
    rand_update_co2_arc_value(co2_arc, co2_value);
    // Update Temperature arc value
    rand_update_temp_arc_value(temp_arc, temp_value);
    // Update Humidity arc value
    rand_update_humid_arc_value(humid_arc, hum_value);
}

void rand_update_co2_arc_value(lv_obj_t *arc, int32_t co2_value)
{
    lv_arc_set_value(arc, co2_value);
    if(co2_value >= 400 && co2_value <= 1000) {
        // Set color to green if value is between 500 and 600
        update_arc_color(arc, lv_color_hex(0x00ff00));
    } else if(co2_value > 1000 && co2_value <= 1200) {
        // Set color to yellow if value is between 600 and 700
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (co2_value > 1200 && co2_value <= 1500) {
        // Set color to red for values outside the range
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%ld", co2_value);
        }
    }
}

void rand_update_humid_arc_value(lv_obj_t *arc, float hum_value)
{
    lv_arc_set_value(arc, hum_value);
    if(hum_value >= 0 && hum_value <= 50) {
        // Set color to green if value is between 500 and 600
        update_arc_color(arc, lv_color_hex(0x00ff00));
    } else if(hum_value > 50 && hum_value <= 60) {
        // Set color to yellow if value is between 600 and 700
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (hum_value > 60 && hum_value <= 90) {
        // Set color to red for values outside the range
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%d%%", (int)hum_value);
        }
    }
}

void rand_update_temp_arc_value(lv_obj_t *arc, float temp_value)
{
    lv_arc_set_value(arc, temp_value);
    if(temp_value >= -10 && temp_value <= 10) {
        // Set color to blue if value is between 500 and 600
        update_arc_color(arc, lv_color_hex(0x0000FF));
    } else if(temp_value > 10 && temp_value <= 20) {
        // Set color to yellow if value is between 600 and 700
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (temp_value > 20 && temp_value <= 30) {
        // Set color to orange for values outside the range
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%d°", (int)temp_value);
        }
    }
}

lv_obj_t *create_dynamic_co2_arc(lv_obj_t *parent)
{
    //set background color
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x11273C), LV_PART_MAIN);

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
    lv_arc_set_value(arc, 400); // Set the current value
    
    lv_arc_set_range(arc, 0, 2300); // Set the range of the arc
    
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
    lv_label_set_text_fmt(num_label, "%ld", lv_arc_get_value(arc));

    // Create the "ppm" label
    lv_obj_t *unit_label = lv_label_create(cont2);
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(unit_label, "PPM");

    lv_obj_align(num_label, LV_ALIGN_CENTER, 0, -10); // Number label
    lv_obj_align(unit_label, LV_ALIGN_CENTER, 0, 20); // "ppm" label


    return arc;
    
}

lv_obj_t *create_dynamic_temp_arc(lv_obj_t *parent)
{
    //set background color
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x11273C), LV_PART_MAIN);

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
    
    lv_arc_set_range(arc, -10, 50); // Set the range of the arc
    
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
    // give me the unicode for degree symbol    

    lv_label_set_text_fmt(num_label, "%ld°", lv_arc_get_value(arc));

    // Create the "ppm" label
    lv_obj_t *unit_label = lv_label_create(cont2);
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(unit_label, "TEMP");

    lv_obj_align(num_label, LV_ALIGN_CENTER, 0, -10); // Number label
    lv_obj_align(unit_label, LV_ALIGN_CENTER, 0, 20); // "ppm" label


    return arc;
    
}

lv_obj_t *create_dynamic_humid_arc(lv_obj_t *parent)
{
    //set background color
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x11273C), LV_PART_MAIN);

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
    
    lv_arc_set_range(arc, 0, 100); // Set the range of the arc
    
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
    // give me the unicode for degree symbol    

    lv_label_set_text_fmt(num_label, "%ld%%", lv_arc_get_value(arc));

    // Create the "ppm" label
    lv_obj_t *unit_label = lv_label_create(cont2);
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(unit_label, "HUMID");

    lv_obj_align(num_label, LV_ALIGN_CENTER, 0, -10); // Number label
    lv_obj_align(unit_label, LV_ALIGN_CENTER, 0, 20); // "ppm" label

    return arc;
    
}

void create_arcs(lv_obj_t *parent, i2c_master_dev_handle_t i2c_handle)
{   
    lv_obj_t *co2_arc;
    lv_obj_t *temp_arc;
    lv_obj_t *hum_arc;

    // Create the arcs
    co2_arc = create_dynamic_co2_arc(parent);
    temp_arc = create_dynamic_temp_arc(parent);
    hum_arc = create_dynamic_humid_arc(parent);

    // Create a timer to update the arcs periodically
    struct lv_timer_data *data = malloc(sizeof(struct lv_timer_data));
    data->i2c_handle = i2c_handle;
    data->co2_arc = co2_arc;
    data->temp_arc = temp_arc;
    data->humid_arc = hum_arc;

    lv_timer_create(update_arc_values, 5000, data); // Update every second
}


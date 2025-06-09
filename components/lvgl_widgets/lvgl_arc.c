
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"

void update_arc_color(lv_obj_t *arc, lv_color_t color)
{
    // Update the arc's color
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
}

void rand_update_co2_arc_value(lv_timer_t *timer)
{
    lv_obj_t *arc = (lv_obj_t *) lv_timer_get_user_data(timer);

    // Randomly update the arc's value
    int32_t new_value = 500 + rand() % 2300; //` Random value between 500 and 2300
    lv_arc_set_value(arc, new_value);
    if(new_value >= 500 && new_value <= 800) {
        // Set color to green if value is between 500 and 600
        ESP_LOGI("LVGL", "Updating arc color to green");
        update_arc_color(arc, lv_color_hex(0x00ff00));
    } else if(new_value > 800 && new_value <= 1200) {
        // Set color to yellow if value is between 600 and 700
        ESP_LOGI("LVGL", "Updating arc color to yellow");
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (new_value > 1200 && new_value <= 1800) {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to orange");
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to red");
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%ld", new_value);
        }
    }
}
void rand_update_humid_arc_value(lv_timer_t *timer)
{
    lv_obj_t *arc = (lv_obj_t *) lv_timer_get_user_data(timer);

    // Randomly update the arc's value
    int32_t new_value = 0 + rand() % 100; //` Random value between 500 and 2300
    lv_arc_set_value(arc, new_value);
    if(new_value >= 0 && new_value <= 50) {
        // Set color to green if value is between 500 and 600
        ESP_LOGI("LVGL", "Updating arc color to green");
        update_arc_color(arc, lv_color_hex(0x00ff00));
    } else if(new_value > 50 && new_value <= 60) {
        // Set color to yellow if value is between 600 and 700
        ESP_LOGI("LVGL", "Updating arc color to yellow");
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (new_value > 60 && new_value <= 90) {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to orange");
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to red");
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%ld%%", new_value);
        }
    }
}

void rand_update_temp_arc_value(lv_timer_t *timer)
{
    lv_obj_t *arc = (lv_obj_t *) lv_timer_get_user_data(timer);

    // Randomly update the arc's value
    int32_t new_value = -50 + rand() % 100; //` Random value between 500 and 2300
    lv_arc_set_value(arc, new_value);
    if(new_value >= -50 && new_value <= 0) {
        // Set color to green if value is between 500 and 600
        ESP_LOGI("LVGL", "Updating arc color to green");
        update_arc_color(arc, lv_color_hex(0x0000FF));
    } else if(new_value > 0 && new_value <= 10) {
        // Set color to yellow if value is between 600 and 700
        ESP_LOGI("LVGL", "Updating arc color to yellow");
        update_arc_color(arc, lv_color_hex(0xffff00));
    } else if (new_value > 10 && new_value <= 20) {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to orange");
        update_arc_color(arc, lv_color_hex(0xff8000)); // Orange color
    } else {
        // Set color to red for values outside the range
        ESP_LOGI("LVGL", "Updating arc color to red");
        update_arc_color(arc, lv_color_hex(0xff0000)); // Red color
    }
    
    // Update the number label
    lv_obj_t *cont = lv_obj_get_child(arc, 0); // assuming container is the first child
    if (cont) {
        lv_obj_t *num_label = lv_obj_get_child(cont, 0);
        lv_obj_t *unit_label = lv_obj_get_child(cont, 1);
        if (num_label) {
            lv_label_set_text_fmt(num_label, "%ld°", new_value);
        }
    }
}

void create_dynamic_co2_arc(lv_obj_t *parent)
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


    // Create a timer to update the arc's value periodically
    lv_timer_create(rand_update_co2_arc_value, 1000, arc); // Update every second
    
}

void create_dynamic_temp_arc(lv_obj_t *parent)
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
    
    lv_arc_set_range(arc, -50, 50); // Set the range of the arc
    
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


    // Create a timer to update the arc's value periodically
    lv_timer_create(rand_update_temp_arc_value, 1000, arc); // Update every second
    
}

void create_dynamic_humid_arc(lv_obj_t *parent)
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


    // Create a timer to update the arc's value periodically
    lv_timer_create(rand_update_humid_arc_value, 1000, arc); // Update every second
    
}


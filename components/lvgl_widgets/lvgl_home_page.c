
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "lvgl_arc.h"
#include "lvgl_home_page.h"

static void settings_btn_event_handler(lv_event_t *e)
{
    // event handler logic
    ESP_LOGI("Settings Button", "Settings button clicked");
}

void top_bar_create(lv_obj_t *parent)
{
    // Create a container for the top bar
    lv_obj_t *top_bar = lv_obj_create(parent);
    lv_obj_set_size(top_bar, 320, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x11273C), LV_PART_MAIN);
    lv_obj_set_style_border_width(top_bar, 0, LV_PART_MAIN);
    lv_obj_set_layout(top_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(top_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_align(top_bar, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);  // Make the top bar itself non-scrollable

    // Left container for settings button
    lv_obj_t *left_cont = lv_obj_create(top_bar);
    lv_obj_set_size(left_cont, 60, 40);
    lv_obj_set_style_bg_opa(left_cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(left_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(left_cont, LV_OBJ_FLAG_SCROLLABLE);  // Non-scrollable

    lv_obj_t *settings_btn = lv_btn_create(left_cont);
    lv_obj_set_size(settings_btn, 40, 40);
    lv_obj_center(settings_btn);
    lv_obj_set_style_bg_color(settings_btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_t *settings_label = lv_label_create(settings_btn);
    lv_label_set_text(settings_label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(settings_label, lv_color_hex(0x11273C), LV_PART_MAIN);
    lv_obj_center(settings_label);
    lv_obj_move_foreground(settings_btn);
    lv_obj_add_event_cb(settings_btn, settings_btn_event_handler, LV_EVENT_PRESSED, NULL);

    // Center label
    lv_obj_t *title_label = lv_label_create(top_bar);
    lv_label_set_text(title_label, "HOME");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(title_label);

    // Right container for battery icon
    lv_obj_t *right_cont = lv_obj_create(top_bar);
    lv_obj_set_size(right_cont, 60, 40);
    lv_obj_set_style_bg_opa(right_cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(right_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(right_cont, LV_OBJ_FLAG_SCROLLABLE);  // Non-scrollable

    lv_obj_t *battery_icon = lv_label_create(right_cont);
    lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(battery_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(battery_icon);
}

void update_battery_icon(lv_obj_t *battery_icon, int battery_level)
{
    // Update the battery icon based on the battery level
    if (battery_level > 75) {
        lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
    } else if (battery_level > 50) {
        lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_3);
    } else if (battery_level > 25) {
        lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_2);
    } else {
        lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_EMPTY);
    }
}

void create_home_page(i2c_master_dev_handle_t i2c_handle)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x11273C), LV_PART_MAIN);

    // Create the top bar
    top_bar_create(lv_scr_act());

    // Create a container for the main content
    lv_obj_t *main_content = lv_obj_create(lv_scr_act());
    lv_obj_set_width(main_content, 320);
    lv_obj_set_height(main_content, 480-40);
    lv_obj_set_style_bg_opa(main_content, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(main_content, 0, LV_PART_MAIN);
    lv_obj_align(main_content, LV_ALIGN_TOP_MID, 0, 40);

    // Use Flex layout to stack arcs vertically
    lv_obj_set_layout(main_content, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_content, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(main_content, 0, LV_PART_MAIN);
    // Add the arcs
    create_arcs(main_content, i2c_handle);
}
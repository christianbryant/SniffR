
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "lvgl_arc.c"

static void settings_btn_event_handler(lv_event_t *e)
{
    // event handler logic
}

void top_bar_create(lv_obj_t *parent)
{
    // Create a container for the top bar
    lv_obj_t *top_bar = lv_obj_create(parent);
    lv_obj_set_size(top_bar, 320, LV_SIZE_CONTENT);
    lv_obj_set_align(top_bar, LV_ALIGN_TOP_MID);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x11273C), LV_PART_MAIN);
    lv_obj_set_style_border_width(top_bar, 0, LV_PART_MAIN);

    // Create a label for the title
    lv_obj_t *title_label = lv_label_create(top_bar);
    lv_label_set_text(title_label, "HOME");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

    // create a button for the settings
    lv_obj_t *settings_btn = lv_btn_create(top_bar);
    lv_obj_set_size(settings_btn, 40, 40);
    lv_obj_set_style_bg_color(settings_btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(settings_btn, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_t *settings_label = lv_label_create(settings_btn);
    lv_label_set_text(settings_label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(settings_label, lv_color_hex(0x11273C), LV_PART_MAIN);
    lv_obj_align(settings_label, LV_ALIGN_CENTER, 0, 0);
    // Add an event handler for the settings button

    lv_obj_add_event_cb(settings_btn, settings_btn_event_handler, LV_EVENT_CLICKED, NULL);

    // Create a battery icon
    lv_obj_t *battery_icon = lv_label_create(top_bar);
    lv_label_set_text(battery_icon, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_size(battery_icon, 40, 40);
    lv_obj_set_style_text_color(battery_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(battery_icon, LV_ALIGN_RIGHT_MID, 10, 0);

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

void create_home_page(lv_obj_t *parent)
{
    lv_obj_t *scr = lv_obj_create(NULL);
    // Create the top bar
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x11273C), LV_PART_MAIN);
    top_bar_create(scr);

    // Create a container for the main content
    lv_obj_t *main_content = lv_obj_create(scr);
    lv_obj_set_size(main_content, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(main_content, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(main_content, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(main_content, 0, LV_PART_MAIN);

    create_dynamic_co2_arc(main_content);

    lv_screen_load(scr);

}
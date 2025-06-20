#include "lvgl.h"
#include "lvgl_settings.h"
#include "nvs_driver.h"
#include "lvgl_home_page.h"
#include "i2c_driver.h"
#include "display_driver.h"
#include "lvgl_arc.h"

int32_t tmp_val;
static lv_obj_t *saved_prev_screen = NULL;
static lv_obj_t * brightness_slider = NULL;

static const char* TAG = "lvgl_settings";

static void switch_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *sw = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);
        int32_t val = state ? 1 : 0;
        if(strcmp(setting_txt, "low_power") == 0 && state == 1){
            save_user_setting("brightness", 50);
            lv_slider_set_value(brightness_slider, 50, LV_ANIM_OFF);
            display_set_brightness(50);
            scd40_low_power_measurement();
            lv_timer_set_period(arc_timer, 30000);
        } else if(strcmp(setting_txt, "low_power") == 0 && state == 0){
            save_user_setting("brightness", 100);
            display_set_brightness(100);
            lv_slider_set_value(brightness_slider, 100, LV_ANIM_OFF);
            scd40_start_measurement();
            lv_timer_set_period(arc_timer, 5000);
        }
        save_user_setting(setting_txt, val);  // Save to NVS
        int32_t debug;
        load_user_setting("debug", &debug, 0);
        if(debug == 1){
            ESP_LOGI(TAG, "%s: %s", setting_txt, state ? "ON" : "OFF");
        }
    }
}

static void dd_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *dd = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        uint16_t selected = lv_dropdown_get_selected(dd);
        int32_t val = (int32_t)selected;
        save_user_setting(setting_txt, val);  // Save to NVS
        if(strcmp(setting_txt, "battery_ver") == 0){
            lv_timer_ready(bat_timer);
        }
        int32_t debug;
        load_user_setting("debug", &debug, 0);
        if(debug == 1){
            ESP_LOGI(TAG, "%s: %d", setting_txt, selected);
        }
    }
}

static void slider_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *slider = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        int val = lv_slider_get_value(slider);
        if(strcmp(setting_txt, "brightness")==0){
            display_set_brightness(val);
        }
        // Update setting here
        save_user_setting(setting_txt, (int32_t)val); // Save to NVS
        int32_t debug;
        load_user_setting("debug", &debug, 0);
        if(debug == 1){
            ESP_LOGI(TAG, "%s: %d", setting_txt, val);
        }
    }
}



static void menu_back_btn_event_cb(lv_event_t *e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED){
        lv_obj_t *main_m = lv_event_get_user_data(e);
        lv_obj_del(main_m);
        lv_scr_load(saved_prev_screen);

        // This will be triggered when root back button is pressed
        int32_t debug;
        load_user_setting("debug", &debug, 0);
        if(debug == 1){
            ESP_LOGI(TAG, "Back button pressed, returning to home screen");
        }
    }
}


lv_obj_t *create_slider(lv_obj_t *parent, const char* label_txt, int curr_val, int min_range, int max_range, lv_event_cb_t event_cb, char* setting_txt){
    lv_obj_t *cont = lv_menu_cont_create(parent);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, label_txt);

    lv_obj_t *slider = lv_slider_create(cont);
    lv_slider_set_range(slider, min_range, max_range);
    lv_slider_set_value(slider, curr_val, LV_ANIM_OFF);
    lv_obj_set_width(slider, 150);
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, setting_txt);
    return slider;
}


lv_obj_t *create_switch(lv_obj_t *parent, const char* label_txt, bool state, lv_event_cb_t event_cb, char* setting_txt){
        lv_obj_t *cont = lv_menu_cont_create(parent);
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, label_txt);

        lv_obj_t *sw = lv_switch_create(cont);
        lv_obj_add_state(sw, state ? LV_STATE_CHECKED : 0);
        lv_obj_add_event_cb(sw, event_cb, LV_EVENT_VALUE_CHANGED, setting_txt);
        return sw;
}



lv_obj_t *create_dropdown(lv_obj_t *parent, const char* label_txt, const char* drop_vals, int val, lv_event_cb_t event_cb, char* setting_txt){
    lv_obj_t *cont = lv_menu_cont_create(parent);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, label_txt);

    lv_obj_t *dd = lv_dropdown_create(cont);
    lv_dropdown_set_options(dd,drop_vals);
    lv_dropdown_set_selected(dd, val); // 0, 1, or 2

    lv_obj_add_event_cb(dd, event_cb, LV_EVENT_VALUE_CHANGED, setting_txt);
    return dd;

}

void create_settings_menu(lv_obj_t *parent) {
    // Create a menu
    saved_prev_screen = parent;
    lv_obj_t *menu = lv_menu_create(NULL);
    lv_scr_load(menu);
    lv_obj_set_size(menu, lv_pct(100), lv_pct(100));
    lv_menu_set_mode_root_back_button(menu, true);
    // Get the back button object
    lv_obj_t* menu_header = lv_menu_get_main_header(menu);
    lv_obj_align(lv_obj_get_child(menu_header,0), LV_ALIGN_LEFT_MID, 0,  0);
    lv_obj_set_flex_align(menu_header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_t *back_btn = lv_menu_get_main_header_back_button(menu);
    lv_obj_set_content_width(back_btn, 20);
    lv_obj_set_content_height(back_btn, 40);
    lv_obj_set_style_pad_gap(menu_header, 100, 0);
    lv_obj_set_flex_align(back_btn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_ext_click_area(back_btn, 175);


    // === Create a settings container page ===
    lv_obj_t *settings_page = lv_menu_page_create(menu, "Settings");

    load_user_setting("brightness", &tmp_val, 100);
    int brightness = tmp_val;
    load_user_setting("low_power", &tmp_val, 0);
    bool low_power = (tmp_val != 0);
    load_user_setting("battery_ver", &tmp_val, 0);
    int battery_ver = tmp_val;
    load_user_setting("debug", &tmp_val, 0);
    bool debug_mode = tmp_val;

    const char* battery_drop = "Voltage\nPercent\nIcon";

    brightness_slider = create_slider(settings_page, "Brightness", brightness, 5, 100, slider_event_cb, "brightness");
    lv_obj_t * power_mode_switch = create_switch(settings_page, "Low Power Mode", low_power, switch_event_cb, "low_power");
    lv_obj_t * debug_mode_switch = create_switch(settings_page, "Debug Mode", debug_mode, switch_event_cb, "debug");
    lv_obj_t * battery_display_dd = create_dropdown(settings_page, "Battery Display", battery_drop, battery_ver, dd_event_cb, "battery_ver");

    lv_obj_add_event_cb(back_btn, menu_back_btn_event_cb, LV_EVENT_CLICKED, menu);
    // === Set it as root page ===
    lv_menu_set_page(menu, settings_page);
}
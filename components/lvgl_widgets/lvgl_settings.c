#include "lvgl.h"
#include "lvgl_settings.h"
#include "nvs_driver.h"
#include "lvgl_home_page.h"
#include "i2c_driver.h"

int32_t tmp_val;
static lv_obj_t *saved_prev_screen = NULL;

static void switch_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *sw = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);
        int32_t val = state ? 1 : 0;
        ESP_LOGI("UI", "%s: %s", setting_txt, state ? "ON" : "OFF");
        save_user_setting(setting_txt, val);  // Save to NVS
    }
}

static void dd_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *dd = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        uint16_t selected = lv_dropdown_get_selected(dd);
        int32_t val = (int32_t)selected;
        ESP_LOGI("UI", "%s: %d", setting_txt, selected);
        save_user_setting(setting_txt, val);  // Save to NVS
    }
}

static void slider_event_cb(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *slider = lv_event_get_target(e);
        const char* setting_txt = lv_event_get_user_data(e);
        int val = lv_slider_get_value(slider);
        // Update setting here
        ESP_LOGI("UI", "%s: %d", setting_txt, val);
        save_user_setting(setting_txt, (int32_t)val); // Save to NVS
    }
}



static void menu_back_btn_event_cb(lv_event_t *e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED){
        lv_obj_t *main_m = lv_event_get_user_data(e);
        lv_obj_del(main_m);
        lv_scr_load(saved_prev_screen);

        // This will be triggered when root back button is pressed
        ESP_LOGI("UI", "Back button pressed, returning to home screen");
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

    lv_obj_t * brightness_slider = create_slider(settings_page, "Brightness", brightness, 0, 100, slider_event_cb, "brightness");
    lv_obj_t * power_mode_switch = create_switch(settings_page, "Low Power Mode", low_power, switch_event_cb, "low_power");
    lv_obj_t * debug_mode_switch = create_switch(settings_page, "Debug Mode", debug_mode, switch_event_cb, "debug");
    lv_obj_t * battery_display_dd = create_dropdown(settings_page, "Battery Display", battery_drop, battery_ver, dd_event_cb, "battery_ver");

    lv_obj_add_event_cb(lv_menu_get_main_header_back_button(menu), menu_back_btn_event_cb, LV_EVENT_CLICKED, menu);
    // === Set it as root page ===
    lv_menu_set_page(menu, settings_page);
}
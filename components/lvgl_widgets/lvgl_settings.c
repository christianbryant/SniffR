#include "lvgl.h"
#include "lvgl_settings.h"
#include "nvs_driver.h"
#include "lvgl_home_page.h"

int32_t tmp_val;
static lv_obj_t *saved_prev_screen = NULL;

static void brightness_slider_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        int val = lv_slider_get_value(slider);
        int32_t brightness_val = (int32_t)val;
        // Update your brightness setting here
        ESP_LOGI("UI", "Brightness set to %d", val);
        // Save to NVS if needed
        save_user_setting("brightness", brightness_val); // Example if you have a save_user_setting function
    }
}

static void low_power_switch_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *sw = lv_event_get_target(e);
        bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);
        int32_t low_power_val = state ? 1 : 0;
        ESP_LOGI("UI", "Low power mode: %s", state ? "ON" : "OFF");
        save_user_setting("low_power", low_power_val);  // Save to NVS
    }
}

static void battery_display_dd_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *dd = lv_event_get_target(e);
        uint16_t selected = lv_dropdown_get_selected(dd);
        int32_t battery_ver_val = (int32_t)selected;
        ESP_LOGI("UI", "Battery display mode set to option %d", selected);
        save_user_setting("battery_ver", battery_ver_val);  // Save to NVS
    }
}
// TODO: Fix this function to approperatly return back to main screen
static void menu_back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *menu = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        // This will be triggered when root back button is pressed
        ESP_LOGI("UI", "Back button pressed, returning to home screen");

        // Delete the settings menu
        lv_obj_del(menu);

        // Show your home screen again (call your home screen init function)
        //create_home_page(lv_scr_act()); // Replace with your actual home screen function
        lv_scr_load(saved_prev_screen);
    }
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
    float brightness = (float)tmp_val;
    load_user_setting("low_power", &tmp_val, 0);
    bool low_power = (tmp_val != 0);
    load_user_setting("battery_ver", &tmp_val, 0);
    int battery_ver = tmp_val;
    // === BRIGHTNESS SLIDER ===
    {
        lv_obj_t *cont = lv_menu_cont_create(settings_page);
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, "Brightness");

        lv_obj_t *slider = lv_slider_create(cont);
        lv_slider_set_range(slider, 0, 100);
        lv_slider_set_value(slider, brightness, LV_ANIM_OFF);
        lv_obj_set_width(slider, 150);

        lv_obj_add_event_cb(slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }

    // === LOW POWER MODE SWITCH ===
    {
        lv_obj_t *cont = lv_menu_cont_create(settings_page);
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, "Low Power Mode");

        lv_obj_t *sw = lv_switch_create(cont);
        lv_obj_add_state(sw, low_power ? LV_STATE_CHECKED : 0);

        lv_obj_add_event_cb(sw, low_power_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }

    // === BATTERY DISPLAY DROPDOWN ===
    {
        lv_obj_t *cont = lv_menu_cont_create(settings_page);
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, "Battery Display");

        lv_obj_t *dd = lv_dropdown_create(cont);
        lv_dropdown_set_options(dd,
            "Voltage\n"
            "Percent\n"
            "Icon"
        );
        lv_dropdown_set_selected(dd, battery_ver); // 0, 1, or 2

        lv_obj_add_event_cb(dd, battery_display_dd_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }
    //lv_obj_add_event_cb(menu, menu_back_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    // === Set it as root page ===
    lv_menu_set_page(menu, settings_page);
}
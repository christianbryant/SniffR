#include "./../components/display/include/st7796_driver.h"
#include "lvgl.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "esp_task_wdt.h"

lv_display_t * my_disp;


void app_main(void) {
    // Watchdog config struct
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 10000,  // 10 seconds
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,  // Monitor all cores
        .trigger_panic = true
    };

    // Initialize the watchdog
    esp_task_wdt_init(&twdt_config);
    esp_task_wdt_add(NULL);
    // Initialize LVGL
    lv_init();
    st7796_init();
    /* Create the LVGL display object and the LCD display driver */
    my_disp = lv_st7796_create(320, 480, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);

    /* Set display orientation to landscape */
    lv_display_set_rotation(my_disp, LV_DISPLAY_ROTATION_90);

    uint32_t buf_size = 320 * 480 * lv_color_format_get_size(lv_display_get_color_format(my_disp));
    uint8_t *buf1 = lv_malloc(buf_size);
    uint8_t *buf2 = lv_malloc(buf_size);
    lv_display_set_buffers(my_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    //ui_init(my_disp);

    lv_obj_t *bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bg, 320, 480);
    lv_obj_set_style_bg_color(bg, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);  // Optional

    // Place the label on top of bg
    lv_obj_t *label = lv_label_create(bg);
    lv_label_set_text(label, "Hello, LVGL!");
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_center(label);
    gpio_set_level(20, 1);

    while (1) {
        lv_timer_handler();
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

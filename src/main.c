
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "display_driver.h"
#include "./../components/lvgl_widgets/lvgl_arc.c"
// #include "./../components/lvgl_widgets/lvgl_home_page.c"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"

#include <inttypes.h>


#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include <sys/cdefs.h>
#include <stdlib.h>

#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_err.h"

void co2_arc_task(void *pvParameters) {
    lv_obj_t *scr = (lv_obj_t *)pvParameters;

    lvgl_port_lock(0);
    create_dynamic_co2_arc();
    lvgl_port_unlock();

    while (1) {
        // Optional: update arc value dynamically
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// void display_main_task(void *pvParameters) {
//     lv_obj_t *scr = (lv_obj_t *)pvParameters;

//     lvgl_port_lock(0);
//     create_home_page(scr);
//     lvgl_port_unlock();

//     while (1) {
//         // Optional: update arc value dynamically
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }



void app_main(void)
{
    display_hw_init();
    // xTaskCreate(
    //     display_main_task,            // Task function
    //     "MyTask",           // Name (for debugging)
    //     4096,               // Stack size (in words, often 4 bytes each)
    //     scr,               // Parameters (optional)
    //     5,                  // Priority (higher = more important)
    //     NULL                // Task handle (optional)s
    // );
    // lvgl_port_lock(0);
    // create_home_page(scr);
    // lvgl_port_unlock();
    lvgl_port_lock(0);
    create_dynamic_co2_arc();
    lvgl_port_unlock();
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("LVGL heap usage: %u bytes used, %u bytes free\n", mon.total_size - mon.free_size, mon.free_size);
    ESP_LOGI("LVGL", "LVGL content added");

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


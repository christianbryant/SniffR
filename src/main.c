
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "display_driver.h"
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





void app_main(void)
{
    display_hw_init();
    lvgl_port_lock(0);
    lv_example_get_started_1();
    lvgl_port_unlock();
    ESP_LOGI("LVGL", "LVGL content added");

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


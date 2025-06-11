#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "battery.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "scd40.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "display_driver.h"
#include "touch_driver.h"
#include "i2c_driver.h"
#include "lvgl_home_page.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"
#include "driver/i2c_master.h"

#include <inttypes.h>


#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include <sys/cdefs.h>
#include <stdlib.h>

#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_err.h"

#define I2C_ADDR_SCD40 0x62
#define I2C_ADDR_TOUCH 0x38
#define I2C_SCL_GPIO GPIO_NUM_7
#define I2C_SDA_GPIO GPIO_NUM_6
static const char *TAG = "MAIN";

void co2_arc_task(void *pvParameters) {
    lv_obj_t *scr = (lv_obj_t *)pvParameters;

    lvgl_port_lock(0);
    create_dynamic_co2_arc(scr);
    lvgl_port_unlock();

    while (1) {
        // Optional: update arc value dynamically
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Declare a global mutex for I2C bus

void app_main(void)
{
    i2c_master_dev_handle_t i2c_touch;
    i2c_master_dev_handle_t i2c_scd40;
    i2c_master_bus_handle_t bus_handle;
    display_hw_init();
    ESP_ERROR_CHECK(i2c_init(I2C_NUM_0, &i2c_scd40,&i2c_touch, &bus_handle, I2C_SDA_GPIO, I2C_SCL_GPIO, I2C_ADDR_SCD40, I2C_ADDR_TOUCH));
    touch_i2c_init(bus_handle);
    ESP_ERROR_CHECK(scd40_init(i2c_scd40));
    lvgl_port_lock(0);
    create_home_page(i2c_scd40);
    lvgl_port_unlock();
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("LVGL heap usage: %u bytes used, %u bytes free\n", mon.total_size - mon.free_size, mon.free_size);
    ESP_LOGI(TAG, "LVGL content added");

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
// void app_main_battery() {
//     int voltage_mv = 0;
//     float voltage = 0.0f;
//     while(1){
//         esp_err_t ret = get_battery_voltage_mv(&voltage_mv, &voltage);
//         if (ret == ESP_OK) {
//             printf("Battery Voltage: %d mV, %.2f V\n", voltage_mv, voltage);
//         } else {
//             printf("Failed to read battery voltage: %s\n", esp_err_to_name(ret));
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
//     }

// }
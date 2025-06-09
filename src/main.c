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
#include "./../Components/scd40/include/scd40.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "display_driver.h"
#include "touch_driver.h"
#include "./../components/lvgl_widgets/lvgl_home_page.c"
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
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_6) | (1ULL << GPIO_NUM_7),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    i2c_master_dev_handle_t i2c_dev;
    display_hw_init();
    touch_i2c_init(I2C_NUM_0, &i2c_dev, GPIO_NUM_6, GPIO_NUM_7);
    lvgl_port_lock(0);
    create_home_page();
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


app_main_scd40_temp{
    i2c_master_dev_handle_t i2c_dev;
    ESP_ERROR_CHECK(scd40_init(I2C_NUM_0, &i2c_dev, GPIO_NUM_6, GPIO_NUM_7));
    scd40_start_measurement(i2c_dev);
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for measurement to be ready
    uint16_t co2;
    float temperature, humidity;
    while(1){
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Delay for 1 second before reading again
        esp_err_t ret = scd40_read_measurement(i2c_dev, &co2, &temperature, &humidity);
        if (ret == ESP_OK) {
            printf("CO2: %d ppm, Temperature: %.2f C, Humidity: %.2f %%\n", co2, temperature, humidity);
        } else {
            printf("Failed to read measurement: %s\n", esp_err_to_name(ret));
        }
    }
}

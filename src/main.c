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


void app_main() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_6) | (1ULL << GPIO_NUM_7),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

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
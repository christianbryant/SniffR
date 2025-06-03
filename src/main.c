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
#include "./../Components/battery/include/battery.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

void app_main() {
    int voltage_mv = 0;
    float voltage = 0.0f;
    while(1){
        esp_err_t ret = get_battery_voltage_mv(&voltage_mv, &voltage);
        if (ret == ESP_OK) {
            printf("Battery Voltage: %d mV, %.2f V\n", voltage_mv, voltage);
        } else {
            printf("Failed to read battery voltage: %s\n", esp_err_to_name(ret));
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }

}
#ifndef BATTERY_H
#define BATTERY_H

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t get_battery_voltage_mv(adc_oneshot_unit_handle_t adc_handle, int *voltage_mv, float *voltage);
int get_battery_percentage(adc_oneshot_unit_handle_t adc_handle);
esp_err_t battery_init();
adc_oneshot_unit_handle_t get_adc_handle();


#ifdef __cplusplus
}
#endif

#endif // SCD40_H


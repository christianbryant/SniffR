#ifndef BATTERY_H
#define BATTERY_H

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t get_battery_voltage_mv(int *voltage_mv, float *voltage);



#ifdef __cplusplus
}
#endif

#endif // SCD40_H


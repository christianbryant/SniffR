#ifndef NVS_DRIVER_H
#define NVS_DRIVER_H

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_check.h"

void init_nvs();

esp_err_t save_user_setting(const char *key, int32_t value);
esp_err_t load_user_setting(const char *key, int32_t *value, int32_t default_val);
esp_err_t factory_reset();

#endif // NVS_DRIVER_H
#include "nvs_flash.h"
#include "nvs_driver.h"


static const char *TAG = "NVS";

typedef struct {
    float brightness;
    int battery_ver;
    bool low_power;
} user_default_t;

user_default_t settings = {
    .brightness = 100.0f,
    .battery_ver = 0,
    .low_power = false
};

void init_nvs()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

esp_err_t save_user_setting(const char *key, int32_t value){
    nvs_handle_t handle;

    esp_err_t err = nvs_open("user_settings", NVS_READWRITE, &handle);

    if (err != ESP_OK) return err;

    err = nvs_set_i32(handle, key, value);
    if (err == ESP_OK) nvs_commit(handle);

    nvs_close(handle);
    return err;
}

esp_err_t load_user_setting(const char *key, int32_t *value, int32_t default_val) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("user_settings", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        *value = default_val;
        return err;
    }

    err = nvs_get_i32(handle, key, value);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *value = default_val;
        ESP_LOGI(TAG, "Setting not found, default applied");
        err = ESP_OK; // Not found, but default is applied
    }

    nvs_close(handle);
    return err;
}

esp_err_t factory_reset(){
    int32_t brightness = (int32_t)settings.brightness;
    int32_t low_power_val = settings.low_power ? 1 : 0;
    int32_t battery_ver_val = (int32_t)settings.battery_ver;
    load_user_setting("brightness", &brightness, settings.brightness);
    load_user_setting("battery_ver", &battery_ver_val, settings.battery_ver);
    load_user_setting("low_power", &low_power_val, settings.low_power);
    return ESP_OK;
}
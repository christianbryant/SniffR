#include "driver/i2c.h"
#include "./include/battery.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"




#define DEFAULT_VREF    1100  // mV
#define ADC_ATTEN       ADC_ATTEN_DB_12  // Updated from DB_11 to DB_12
#define BATTERY_ADC_UNIT ADC_UNIT_1
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_2 // GPIO2 on ADC1

static const char *TAG = "Battery";

int get_battery_percentage(adc_oneshot_unit_handle_t adc_handle){
    int voltage_mv;
    float voltage_v;
    get_battery_voltage_mv(adc_handle, &voltage_mv, &voltage_v);
    float percent = (voltage_v - 3.2) / (4.2 - 3.2) * 100.0;
    if(percent > 100.0){
        percent = 100.0;
    }else if (percent < 0){
        percent = 0;
    }
    return (int)percent;
}

esp_err_t battery_init(adc_oneshot_unit_handle_t *adc_handle){
    esp_err_t ret;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = BATTERY_ADC_UNIT,
    };
    ret = adc_oneshot_new_unit(&init_config, adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC unit: %s", esp_err_to_name(ret));
        return ret;
    }

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_oneshot_config_channel(*adc_handle, BATTERY_ADC_CHANNEL, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC channel: %s", esp_err_to_name(ret));
        adc_oneshot_del_unit(*adc_handle);
        return ret;
    }
    return ESP_OK;
}

esp_err_t get_battery_voltage_mv(adc_oneshot_unit_handle_t adc_handle, int *voltage_mv, float *voltage) {
    esp_err_t ret;
    int raw = 0;
    ret = adc_oneshot_read(adc_handle, BATTERY_ADC_CHANNEL, &raw);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(ret));
        adc_oneshot_del_unit(adc_handle);
        return ret;
    }

    // Apply ADC calibration
    adc_cali_handle_t cali_handle = NULL;
    bool do_calibration = false;
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = BATTERY_ADC_UNIT,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    if (adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle) == ESP_OK) {
        do_calibration = true;
    }

    if (do_calibration) {
        ret = adc_cali_raw_to_voltage(cali_handle, raw, voltage_mv);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "ADC calibration failed, using default Vref");
            *voltage_mv = raw * DEFAULT_VREF / 4095;
        }
        adc_cali_delete_scheme_curve_fitting(cali_handle);
    } else {
        // fallback calculation
        *voltage_mv = raw * DEFAULT_VREF / 4095;
    }
    // convert the resisted voltage to true battery voltage
    *voltage_mv = (*voltage_mv * 2); // Assuming a voltage divider with equal resistors
    *voltage = *voltage_mv / 1000.0; // Convert to volts
    if (*voltage < 3.0) {
        ESP_LOGW(TAG, "Battery voltage is low: %.2f V", *voltage);
    } else if (*voltage > 4.2) {
        ESP_LOGW(TAG, "Battery voltage is high: %.2f V", *voltage);
    }

    ESP_LOGI(TAG, "Battery voltage: %d mV", *voltage_mv);
    return ESP_OK;
}
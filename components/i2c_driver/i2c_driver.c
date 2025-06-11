#include <esp_err.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "i2c_driver.h"
#include "esp_check.h"



esp_err_t i2c_init(i2c_port_t i2c_num, i2c_master_dev_handle_t *i2c_dev1, i2c_master_dev_handle_t *i2c_dev2, i2c_master_bus_handle_t *bus_handle, gpio_num_t sda_pin, gpio_num_t scl_pin, uint16_t dev_addr1, uint16_t dev_addr2)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_6) | (1ULL << GPIO_NUM_7),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };


    ESP_ERROR_CHECK(gpio_config(&io_conf));
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = i2c_num,
        .scl_io_num = scl_pin,
        .sda_io_num = sda_pin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, bus_handle));

    i2c_device_config_t dev_cfg1 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr1, // Example address
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg1, i2c_dev1));
    if (*i2c_dev1 == NULL || bus_handle == NULL) {
        ESP_LOGE("I2C", "Failed to add I2C device");
        return ESP_FAIL;
    }

    i2c_device_config_t dev_cfg2 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr2, // Example address
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg2, i2c_dev2));

    
    if (*i2c_dev2 == NULL || bus_handle == NULL) {
        ESP_LOGE("I2C", "Failed to add I2C device");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}
#ifndef SCD40_H
#define SCD40_H

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the SCD40 sensor (I2C master config)
esp_err_t scd40_init(i2c_port_t i2c_num, i2c_master_dev_handle_t *i2c_dev, gpio_num_t sda_pin, gpio_num_t scl_pin);

// Deinitialize the SCD40 sensor (remove I2C driver)
esp_err_t scd40_deinit(i2c_master_dev_handle_t i2c_num);

// Sensor control commands
esp_err_t scd40_start_measurement(i2c_master_dev_handle_t handle);
esp_err_t scd40_low_power_measurement(i2c_port_t i2c_num);
esp_err_t scd40_stop_measurement(i2c_port_t i2c_num);
esp_err_t scd40_reset(i2c_port_t i2c_num);
esp_err_t scd40_wake_up(i2c_port_t i2c_num);
esp_err_t scd40_power_down(i2c_port_t i2c_num);

// Perform forced recalibration with a target CO2 ppm value
esp_err_t scd40_force_recalibration(i2c_port_t i2c_num, uint16_t co2_value);

// Read the latest measurement: CO2 ppm, temperature (°C), humidity (%RH)
esp_err_t scd40_read_measurement(i2c_master_dev_handle_t handle, uint16_t *co2, float *temperature, float *humidity);

// Log sensor type and serial number (for debugging)
void scd40_log_sensor_type(i2c_port_t i2c_num);

// CRC8 function for data integrity checks (polynomial 0x31)
uint8_t crc8(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // SCD40_H
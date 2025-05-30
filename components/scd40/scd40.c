#include "driver/i2c.h"
#include "./include/scd40.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

// TODO: Update CRC to do the check and error handling
// Create this as a class in the future


#define SCD40_I2C_ADDRESS 0x62


#define SCD40_CMD_WAKE_UP 0x36f6
#define SCD40_CMD_POWER_DOWN 0x36a0
#define SCD40_CMD_RESET 0xD304


#define SCD40_CMD_START_MEASUREMENT 0x21B1
#define SCD40_CMD_STOP_MEASUREMENT 0x3F86
#define SCD40_CMD_READ_MEASUREMENT 0xEC05
#define SCD40_CMD_READ_READY_STATUS 0xE4B8


#define SCD40_CMD_SET_MEASUREMENT_INTERVAL 0x4600
#define SCD40_CMD_SET_AUTO_SELF_CALIBRATION 0x5306


#define SCD40_CMD_READ_SERIAL_NUMBER 0x3682
#define SCD40_CMD_READ_SENSOR_VARIANT 0x202F


#define SCD40_CMD_SET_FORCED_RECALIBRATION 0x362F


#define SCD40_CMD_READ_AUTO_SELF_CALIBRATION 0xE200


#define SCD40_CMD_READ_FORCED_RECALIBRATION 0xE2F0
#define SCD40_CMD_READ_SERIAL_NUMBER_LENGTH 9



#define SCD40_CMD_START_LOW_POWER_MEASUREMENT 0x21AC
#define SCD40_CMD_STOP_LOW_POWER_MEASUREMENT 0x3F86

//define tag for logging
static const char *TAG = "SCD40";


esp_err_t scd40_init(i2c_port_t i2c_num, gpio_num_t sda_pin, gpio_num_t scl_pin) {
    // Initialize the I2C bus
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000, // 100 kHz
    };
    ESP_ERROR_CHECK(i2c_param_config(i2c_num, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_num, conf.mode, 0, 0, 0));
    return ESP_OK;
}

esp_err_t scd40_deinit(i2c_port_t i2c_num) {
    // Deinitialize the I2C bus
    ESP_ERROR_CHECK(i2c_driver_delete(i2c_num));
    return ESP_OK;
}

esp_err_t scd40_start_measurement(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_START_MEASUREMENT >> 8, SCD40_CMD_START_MEASUREMENT & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t scd40_low_power_measurement(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_START_LOW_POWER_MEASUREMENT >> 8, SCD40_CMD_START_LOW_POWER_MEASUREMENT & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t scd40_stop_measurement(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_STOP_MEASUREMENT >> 8, SCD40_CMD_STOP_MEASUREMENT & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t scd40_reset(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_RESET >> 8, SCD40_CMD_RESET & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t scd40_wake_up(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_WAKE_UP >> 8, SCD40_CMD_WAKE_UP & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t scd40_power_down(i2c_port_t i2c_num) {
    uint8_t data[2] = {SCD40_CMD_POWER_DOWN >> 8, SCD40_CMD_POWER_DOWN & 0xFF};
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

// CRC8 calculation function for SCD40 sensor
uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}

esp_err_t scd40_force_recalibration(i2c_port_t i2c_num, uint16_t co2_value) {
    uint8_t data[4];
    data[0] = SCD40_CMD_SET_FORCED_RECALIBRATION >> 8;
    data[1] = SCD40_CMD_SET_FORCED_RECALIBRATION & 0xFF;
    data[2] = (co2_value >> 8) & 0xFF; // MSB
    data[3] = co2_value & 0xFF; // LSB
    data[4] = crc8(data + 3, 2); // CRC for the data
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    vTaskDelay(400 / portTICK_PERIOD_MS); // Wait for the sensor to process the command
    uint8_t response[3];
    i2c_master_read_from_device(i2c_num, SCD40_I2C_ADDRESS, response, sizeof(response), 1000 / portTICK_PERIOD_MS);
    // crc check
    if (crc8(response + 3, 2) != response[2]) {
        ESP_LOGE(TAG, "Forced recalibration CRC check failed");
        return ESP_FAIL;
    }
    if (((response[0] << 8) | data[1]) == 0xFFFF){
        ESP_LOGE(TAG, "Forced recalibration failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Forced recalibration successful, CO2 value set to %d ppm", co2_value);
    return ESP_OK;
}

// I2C function to read the measurement from the SCD40 sensor
// Response format:
// [CO2 (2 bytes), CO2 CRC (1 byte), Temperature (2 bytes), Temperature CRC (1 byte), Humidity (2 bytes), Humidity CRC (1 byte)]
esp_err_t scd40_read_measurement(i2c_port_t i2c_num, uint16_t *co2, float *temperature, float *humidity) {
    uint8_t data[9]; // 2 bytes for CO2, 2 bytes for temperature, 2 bytes for humidity, 3 bytes for CRCs
    uint8_t cmd[2] = {SCD40_CMD_READ_READY_STATUS >> 8, SCD40_CMD_READ_READY_STATUS & 0xFF};
    uint8_t cmd2[2] = {SCD40_CMD_READ_MEASUREMENT >> 8, SCD40_CMD_READ_MEASUREMENT & 0xFF};

    // Check if the sensor is ready for measurement
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, cmd, sizeof(cmd), 1000 / portTICK_PERIOD_MS);
    vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for the sensor to process the command
    i2c_master_read_from_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    if (data[0] != 0x00) {
        ESP_LOGE(TAG, "Sensor not ready for measurement");
        return ESP_ERR_NOT_FOUND;
    }
    
    // Send the read measurement command
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, cmd2, sizeof(cmd2), 1000 / portTICK_PERIOD_MS);

    // Wait for the measurement to be ready
    vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust delay as necessary for your application
    
    // Read the measurement data
    i2c_master_read_from_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    
    // Check CRCs for CO2, temperature, and humidity
    if (crc8(data, 2) != data[2]) {
        ESP_LOGE(TAG, "CO2 CRC check failed");
        return ESP_ERR_NOT_FOUND;
    }
    if (crc8(data + 3, 2) != data[5]) {
        ESP_LOGE(TAG, "Temperature CRC check failed");
        return ESP_ERR_NOT_FOUND;
    }
    if (crc8(data + 6, 2) != data[8]) {
        ESP_LOGE(TAG, "Humidity CRC check failed");
        return ESP_ERR_NOT_FOUND;
    }
    // Extract CO2, temperature, and humidity values
    // CO2 is direct ppm value
    *co2 = (data[0] << 8) | data[1];

    // Conversion given from the SCD40 datasheet for temperature and humidity
    // Temperature is in Celsius, calculated from the raw value
    // Formula: T = -45 + 175 * (raw_value / (2^16 - 1))
    *temperature = -45.0f +175.0f * (((data[3] << 8) | data[4])/((1 << 16)-1.0f));

    // Humidity is in percentage, calculated from the raw value
    // Formula: RH = 100 * (raw_value / (2^16 - 1)
    *humidity = 100 * (((data[6] << 8) | data[7])/ ((1 << 16)-1.0f));
    ESP_LOGI(TAG, "CO2: %d ppm, Temperature: %.2f C, Humidity: %.2f %%", *co2, *temperature, *humidity);
    return ESP_OK;
}

// Function to log the sensor type and serial number
// Serial number format response: Data[0-1] CRC[2] Data[3-4] CRC[5] Data[6-7] CRC[8]
void scd40_log_sensor_type(i2c_port_t i2c_num) {

    uint8_t cmd[2] = {SCD40_CMD_READ_SERIAL_NUMBER >> 8, SCD40_CMD_READ_SERIAL_NUMBER & 0xFF};
    uint8_t data[SCD40_CMD_READ_SERIAL_NUMBER_LENGTH];

    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, cmd, sizeof(cmd), 1000 / portTICK_PERIOD_MS);
    vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for the sensor to process the command
    i2c_master_read_from_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    //crc check
    for (int i = 0; i < SCD40_CMD_READ_SERIAL_NUMBER_LENGTH - 1;) {
        if (crc8(data + i, 2) != data[i + 2]) {
            ESP_LOGE(TAG, "Serial number CRC check failed at block %d", i / 3);
            return;
        }
        i += 3; // Move to the next data block (2 bytes of data + 1 byte of CRC)
    }
    // Log the serial number
    ESP_LOGI(TAG, "Serial Number: %02X%02X%02X%02X%02X%02X",
             data[0], data[1], data[3], data[4], data[6], data[7]);
    
    // Verify Sensor Type
    //reuse cmd and data arrays
    cmd[0] = SCD40_CMD_READ_SENSOR_VARIANT >> 8;
    cmd[1] = SCD40_CMD_READ_SENSOR_VARIANT & 0xFF;
    i2c_master_write_to_device(i2c_num, SCD40_I2C_ADDRESS, cmd, sizeof(cmd), 1000 / portTICK_PERIOD_MS);
    vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for the sensor to process the command
    i2c_master_read_from_device(i2c_num, SCD40_I2C_ADDRESS, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    //crc check
    if( crc8(data, 2) != data[2]) {
        ESP_LOGE(TAG, "Sensor variant CRC check failed");
        return;
    }
    if ((data[0] & 0xF0) >> 4 == 0x00) {
        ESP_LOGI(TAG, "Sensor Type: SCD40");
    } else if ((data[0] & 0xF0) >> 4 == 0x01) {
        ESP_LOGI(TAG, "Sensor Type: SCD41");
        ESP_LOGE(TAG, "Sensor variant not supported, please use SCD40 instead");
        return;
    } else if ((data[0] & 0xF0) >> 4 == 0x05 ) {
        ESP_LOGI(TAG, "Sensor Type: SCD43");
        ESP_LOGE(TAG, "Sensor variant not supported, please use SCD40 instead");
    } else {
        // include the data
        // If the sensor type is not recognized, log an error
        ESP_LOGE(TAG, "Sensor Type: Unknown (%02X)", (data[0] & 0xF0) >> 4);
        return;
    }
}
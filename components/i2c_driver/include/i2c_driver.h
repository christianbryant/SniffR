#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <esp_err.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"



esp_err_t i2c_init(i2c_port_t i2c_num, i2c_master_dev_handle_t *i2c_dev1, i2c_master_dev_handle_t *i2c_dev2, i2c_master_bus_handle_t *bus_handle, gpio_num_t sda_pin, gpio_num_t scl_pin, uint16_t dev_addr1, uint16_t dev_addr2);

#endif // I2C_DRIVER_H
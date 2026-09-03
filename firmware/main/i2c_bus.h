#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief I2C bus init.
     *
     * @param bus_handle I2C bus handle.
     * @return esp_err_t
     */
    esp_err_t i2c_bus_init(i2c_master_bus_handle_t *bus_handle);

    /**
     * @brief I2C bus deinit.
     *
     * @param bus_handle I2C bus handle.
     * @return esp_err_t
     */
    esp_err_t i2c_bus_deinit(i2c_master_bus_handle_t *bus_handle);

    /**
     * @brief I2 bus add device.
     *
     * @param bus_handle I2C bus handle.
     * @param dev_addr I2C device address.
     * @param scl_speed_hz I2C device clock speed.
     * @param dev_handle I2C device handle.
     * @return esp_err_t
     */
    esp_err_t i2c_bus_add_device(i2c_master_bus_handle_t bus_handle, bool ten_bit_addr, uint16_t dev_addr, uint16_t scl_speed_hz, i2c_master_dev_handle_t *dev_handle);

    /**
     * @brief I2 bus remove device.
     *
     * @param dev_handle I2C device handle.
     * @return esp_err_t
     */
    esp_err_t i2c_bus_remove_device(i2c_master_dev_handle_t *dev_handle);

    /**
     * @brief I2C bus scan 7bit and 10bit.
     * 
     * @param dev_handle I2C device handle.
     * @return esp_err_t 
     */
    esp_err_t i2c_bus_scan(i2c_master_bus_handle_t *bus_handle);

#ifdef __cplusplus
}
#endif
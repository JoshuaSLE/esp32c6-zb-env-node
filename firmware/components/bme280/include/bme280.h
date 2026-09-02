#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct bme280_handle *bme280_handle_t;

    typedef struct bme280_config
    {
        i2c_master_bus_handle_t bus_handle;
        uint16_t i2c_address;
        uint8_t temp_over_sample;
        uint8_t pres_over_sample;
        uint8_t hum_over_sample;
    } bme280_config_t;

    /**
     * @brief BME280 init.
     *
     * @param config BME280 configuration struct.
     * @param handle BME280 handle.
     * @return esp_err_t
     */
    esp_err_t bme280_init(bme280_config_t *config, bme280_handle_t *ret_handle);

    esp_err_t bme280_deinit(bme280_handle_t *handle);

#ifdef __cplusplus
}
#endif
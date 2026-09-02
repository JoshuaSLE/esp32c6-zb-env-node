#include "bme280.h"

#include "esp_check.h"

typedef struct bme280_comp
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_T4;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme280_comp_t;

struct bme280_handle
{
    i2c_master_dev_handle_t i2c_device;
    bme280_comp_t comp;
};

esp_err_t bme280_init(bme280_config_t *config, bme280_handle_t *ret_handle)
{
    if (config == NULL || ret_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    bme280_handle_t *dev_handle = calloc(1, sizeof(bme280_handle_t));
    if (dev_handle == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    if (ret != ESP_OK)
    {
        free(dev_handle);
        return ret;
    }

    *ret_handle = (bme280_handle_t)dev_handle;
    return ESP_OK;
}

esp_err_t bme280_deinit(bme280_handle_t *handle)
{
    if (handle == NULL || *handle == NULL)
    {
        return ESP_OK;
    }

    return ESP_OK;
}
#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#define BME280_REG_ID 0xD3
#define BME280_REG_REST 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_STATUS 0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_PRESS 0xF7
#define BME280_REG_TEMP 0xFA
#define BME280_REG_HUM 0xFE
#define BME280_REG_HUM 0xFE

#define BME280_ID_VAL 0x60
#define BME280_SOFT_RESET_VAL 0xB6

#define BME280_STATUS_MEAS 0x04
#define BME280_STATUS_UPDATE 0x01

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief BME280 temperature over sampling.
     *
     */
    typedef enum bme280_temp_over_sample
    {
        BME280_TEMP_OVER_SAMPLE_X0,
        BME280_TEMP_OVER_SAMPLE_X1,
        BME280_TEMP_OVER_SAMPLE_X2,
        BME280_TEMP_OVER_SAMPLE_X4,
        BME280_TEMP_OVER_SAMPLE_X8,
        BME280_TEMP_OVER_SAMPLE_X16,
    } bme280_temp_over_sample_t;

    /**
     * @brief BME280 humidity over sampling.
     *
     */
    typedef enum bme280_hum_over_sample
    {
        BME280_HUM_OVER_SAMPLE_X0,
        BME280_HUM_OVER_SAMPLE_X1,
        BME280_HUM_OVER_SAMPLE_X2,
        BME280_HUM_OVER_SAMPLE_X4,
        BME280_HUM_OVER_SAMPLE_X8,
        BME280_HUM_OVER_SAMPLE_X16,
    } bme280_hum_over_sample_t;

    /**
     * @brief BME280 pressure over sampling.
     *
     */
    typedef enum bme280_press_over_sample
    {
        BME280_PRESS_OVER_SAMPLE_X0,
        BME280_PRESS_OVER_SAMPLE_X1,
        BME280_PRESS_OVER_SAMPLE_X2,
        BME280_PRESS_OVER_SAMPLE_X4,
        BME280_PRESS_OVER_SAMPLE_X8,
        BME280_PRESS_OVER_SAMPLE_X16,
    } bme280_press_over_sample_t;

    /**
     * @brief BME280 mode.
     *
     */
    typedef enum bme280_mode
    {
        BME280_MODE_SLEEP,
        BME280_MODE_FORCED,
        BME280_MODE_NORMAL,
    } bme280_mode_t;

    /**
     * @brief BME280 standby time (ms).
     *
     */
    typedef enum bme280_standby
    {
        BME280_STANDBY_MS_0_5,
        BME280_STANDBY_MS_62_5,
        BME280_STANDBY_MS_125,
        BME280_STANDBY_MS_250,
        BME280_STANDBY_MS_500,
        BME280_STANDBY_MS_1000,
        BME280_STANDBY_MS_10000,
        BME280_STANDBY_MS_20000,
    } bme280_standby_t;

    /**
     * @brief BME280 filter.
     *
     */
    typedef enum bme280_filter
    {
        BME280_FILTER_0,
        BME280_FILTER_2,
        BME280_FILTER_4,
        BME280_FILTER_8,
        BME280_FILTER_16,
    } bme280_filter_t;

    /**
     * @brief BME280 handle.
     *
     */
    typedef struct bme280_handle *bme280_handle_t;

    /**
     * @brief BME280 configuration struct.
     *
     */
    typedef struct bme280_config
    {
        i2c_master_bus_handle_t bus_handle;
        uint16_t i2c_address;
        bme280_mode_t mode;
        bme280_temp_over_sample_t temp_over_sample;
        bme280_hum_over_sample_t hum_over_sample;
        bme280_press_over_sample_t pres_over_sample;
        bme280_standby_t standby;
        bme280_filter_t filter;
    } bme280_config_t;

    /**
     * @brief BME280 init.
     *
     * @param config BME280 configuration struct.
     * @param handle BME280 handle.
     * @return esp_err_t
     */
    esp_err_t bme280_init(const bme280_config_t *config, bme280_handle_t *ret_handle);

    /**
     * @brief BME280 deinit.
     *
     * @param handle BME280 handle.
     * @return esp_err_t
     */
    esp_err_t bme280_deinit(bme280_handle_t *handle);

#ifdef __cplusplus
}
#endif
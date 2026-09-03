#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#define BME280_REG_ID 0xD0
#define BME280_REG_RESET 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_STATUS 0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_PRESS 0xF7
#define BME280_REG_TEMP 0xFA
#define BME280_REG_HUM 0xFE

#define BME280_REG_CALIB00 0x88
#define BME280_REG_CALIB26 0xE1

#define BME280_ID_VAL 0x60
#define BME280_SOFT_RESET_VAL 0xB6

#define BME280_STATUS_MEAS BIT(3)
#define BME280_STATUS_UPDATE BIT(0)

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief BME280 handle.
     *
     */
    typedef struct bme280_handle *bme280_handle_t;

    /**
     * @brief BME280 temperature over sampling.
     *
     */
    typedef enum bme280_over_sample
    {
        BME280_OVER_SAMPLING_SKIP,
        BME280_OVER_SAMPLE_1X,
        BME280_OVER_SAMPLE_2X,
        BME280_OVER_SAMPLE_4X,
        BME280_OVER_SAMPLE_8X,
        BME280_OVER_SAMPLE_16X,
    } bme280_over_sample_t;

    /**
     * @brief BME280 mode.
     *
     * @note BME280_MODE_FORCED performs a single measurement then returns
     *       to sleep automatically. Callers using forced mode must call
     *       @ref bme280_trigger_measurement before each @ref bme280_read_data
     *       to start a new conversion, since the sensor will otherwise just
     *       return the previous (stale) conversion result.
     */
    typedef enum bme280_mode
    {
        BME280_MODE_SLEEP = 0x00,
        BME280_MODE_FORCED = 0x01,
        BME280_MODE_NORMAL = 0x03,
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
        BME280_STANDBY_MS_10,
        BME280_STANDBY_MS_20,
    } bme280_standby_t;

    /**
     * @brief BME280 filter.
     *
     */
    typedef enum bme280_filter
    {
        BME280_FILTER_OFF,
        BME280_FILTER_2,
        BME280_FILTER_4,
        BME280_FILTER_8,
        BME280_FILTER_16,
    } bme280_filter_t;

    /**
     * @brief BME280 configuration struct.
     *
     */
    typedef struct bme280_config
    {
        i2c_master_bus_handle_t bus_handle;
        uint16_t i2c_address;
        uint32_t scl_speed_hz;
        bme280_mode_t mode;
        bme280_over_sample_t temp_over_sample;
        bme280_over_sample_t hum_over_sample;
        bme280_over_sample_t pres_over_sample;
        bme280_standby_t standby;
        bme280_filter_t filter;
    } bme280_config_t;

    /**
     * @brief BME280 data.
     *
     */
    typedef struct bme280_data
    {
        float temp;  // Degrees Celsius
        float press; // hPa
        float hum;   // % Relative Humidity
    } bme280_data_t;

    /**
     * @brief BME280 init.
     *
     * @param config BME280 configuration struct.
     * @param ret_handle @ref bme280_handle_t.
     * @return esp_err_t
     */
    esp_err_t bme280_init(const bme280_config_t *config, bme280_handle_t *ret_handle);

    /**
     * @brief BME280 deinit.
     *
     * @param handle @ref bme280_handle_t.
     * @return esp_err_t
     */
    esp_err_t bme280_deinit(bme280_handle_t *handle);

    /**
     * @brief Trigger a single measurement while in forced mode.
     *
     * @note Only needed when the device was configured with
     *       BME280_MODE_FORCED. Has no effect (and is unnecessary) in
     *       normal mode, where the sensor free-runs on its own schedule.
     *       Blocks (via internal poll/delay) until the conversion completes
     *       or CONFIG_APP_I2C_TIMEOUT_MS elapses.
     *
     * @param handle @ref bme280_handle_t.
     * @return esp_err_t
     */
    esp_err_t bme280_trigger_measurement(bme280_handle_t handle);

    /**
     * @brief BME280 read data and compensate them.
     *
     * @param handle @ref bme280_handle_t.
     * @param data @ref bme280_data_t
     * @return esp_err_t
     *
     */
    esp_err_t bme280_read_data(bme280_handle_t handle, bme280_data_t *data);

#ifdef __cplusplus
}
#endif
idf_component_register(SRCS "bme280.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_driver_i2c esp_timer)

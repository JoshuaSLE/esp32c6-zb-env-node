idf_component_register(SRCS "presence.c" "display.c" "i2c_bus.c" "env_mon.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_i2c esp_lcd esp_driver_gpio
                        bme280 vcnl4010)

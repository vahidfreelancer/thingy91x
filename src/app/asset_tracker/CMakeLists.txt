# src/app/asset_tracker/CMakeLists.txt

target_sources(app PRIVATE
    app.c
    ../../platform/drivers/high_g/high_g.c
    ../../platform/drivers/pmic/pmic.c
    ../../platform/drivers/led/led.c
    ../../platform/drivers/buttons/buttons.c
    ../../platform/drivers/magnetometer/magnetometer.c
    ../../platform/drivers/wifi_scan/wifi_scan.c
    ../../platform/drivers/cellular/cellular.c
)

target_include_directories(app PRIVATE
    .
    ../../platform/drivers/high_g
    ../../platform/drivers/pmic
    ../../platform/drivers/led
    ../../platform/drivers/buttons
    ../../platform/drivers/magnetometer
    ../../platform/drivers/wifi_scan
    ../../platform/drivers/cellular
)

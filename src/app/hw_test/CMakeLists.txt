# src/app/hw_test/CMakeLists.txt

target_sources(app PRIVATE
    app.c
    ../../platform/drivers/environment/environment.c
    ../../platform/drivers/high_g/high_g.c
    ../../platform/drivers/pmic/pmic.c
    ../../platform/drivers/led/led.c
    ../../platform/drivers/buttons/buttons.c
    ../../platform/drivers/magnetometer/magnetometer.c
    ../../platform/drivers/wifi_scan/wifi_scan.c
    ../../platform/drivers/cellular/cellular.c
    ../../services/fusion/ekf_fusion.c
)

target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/environment
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/high_g
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/pmic
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/led
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/buttons
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/magnetometer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/wifi_scan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/drivers/cellular
    ${CMAKE_CURRENT_SOURCE_DIR}/../../services/fusion
)

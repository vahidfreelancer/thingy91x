# src/app/autopilot/CMakeLists.txt

target_sources(app PRIVATE
    app.c
    ../../platform/drivers/imu/imu.c
    ../../libraries/math/matrix.c
    ../../libraries/math/quaternion.c
    ../../libraries/ekf/ekf.c
)

target_include_directories(app PRIVATE
    .
    ../../platform/drivers/imu
    ../../libraries/math
    ../../libraries/ekf
)


# src/app/travel_history/CMakeLists.txt

# Add application sources, along with platform drivers, services, and libraries
target_sources(app PRIVATE
    app.c
    ../../platform/drivers/imu/imu.c
    ../../platform/drivers/gnss/gnss.c
    ../../services/storage/storage.c
    ../../libraries/attitude/attitude.c
    ../../libraries/kalman/kalman.c
)

# Include search paths for drivers, services, and general libraries
target_include_directories(app PRIVATE
    .
    ../../platform/drivers/imu
    ../../platform/drivers/gnss
    ../../services/storage
    ../../libraries/attitude
    ../../libraries/kalman
)

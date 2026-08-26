# src/app/env_monitor/CMakeLists.txt

target_sources(app PRIVATE
    app.c
    ../../platform/drivers/environment/environment.c
)

target_include_directories(app PRIVATE
    .
    ../../platform/drivers/environment
)

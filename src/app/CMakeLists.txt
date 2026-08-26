# src/app/CMakeLists.txt

# Add shared entry point
target_sources(app PRIVATE main.c)

# Include header search path for the active app profile interface (app.h)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Conditionally compile the selected application profile
if(CONFIG_APP_PROFILE_HELLO_WORLD)
    add_subdirectory(hello_world)
elseif(CONFIG_APP_PROFILE_ASSET_TRACKER)
    add_subdirectory(asset_tracker)
elseif(CONFIG_APP_PROFILE_AUTOPILOT)
    add_subdirectory(autopilot)
elseif(CONFIG_APP_PROFILE_ENV_MONITOR)
    add_subdirectory(env_monitor)
elseif(CONFIG_APP_PROFILE_TRAVEL_HISTORY)
    add_subdirectory(travel_history)
elseif(CONFIG_APP_PROFILE_HW_TEST)
    add_subdirectory(hw_test)
else()
    message(FATAL_ERROR "No active application profile selected in Kconfig!")
endif()

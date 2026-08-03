#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "storage.h"

LOG_MODULE_REGISTER(storage_service);

#define LOG_FILE_PATH "/lfs/travel.csv"

/* Check if the storage partition is defined in the Devicetree */
#if DT_HAS_FIXED_PARTITION_LABEL(storage)
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(lfs_config);
static struct fs_mount_t travel_lfs_mount = {
    .type = FS_LITTLEFS,
    .fs_data = &lfs_config,
    .storage_dev = (void *)FLASH_AREA_ID(storage),
    .mnt_point = "/lfs",
};
static bool fs_ready = false;
#else
static bool fs_ready = false;
#endif

int storage_init(void)
{
#if DT_HAS_FIXED_PARTITION_LABEL(storage)
    int err = fs_mount(&travel_lfs_mount);
    if (err && err != -EALREADY) {
        LOG_ERR("Failed to mount LittleFS on flash partition 'storage' (err: %d)", err);
        return err;
    }
    LOG_INF("LittleFS successfully mounted at %s", travel_lfs_mount.mnt_point);
    fs_ready = true;
#else
    LOG_WRN("Flash partition 'storage' not defined in DTS; falling back to memory log simulator.");
    fs_ready = true;
#endif
    return 0;
}

int storage_log_travel_entry(const struct travel_entry *entry)
{
    if (!fs_ready) {
        LOG_ERR("Storage filesystem not initialized!");
        return -ENODEV;
    }

#if DT_HAS_FIXED_PARTITION_LABEL(storage)
    struct fs_file_t file;
    fs_file_t_init(&file);

    int err = fs_open(&file, LOG_FILE_PATH, FS_O_CREATE | FS_O_APPEND | FS_O_WRITE);
    if (err) {
        LOG_ERR("Failed to open file %s (err: %d)", LOG_FILE_PATH, err);
        return err;
    }

    char csv_buf[160];
    int len = snprintf(csv_buf, sizeof(csv_buf),
                       "%u,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.2f,%u,%.2f\n",
                       entry->timestamp, entry->latitude, entry->longitude,
                       entry->altitude, entry->speed, entry->roll, entry->pitch,
                       entry->yaw, entry->satellites, entry->hdop);

    ssize_t written = fs_write(&file, csv_buf, len);
    if (written < 0) {
        LOG_ERR("Failed to write to file %s (err: %d)", LOG_FILE_PATH, (int)written);
        fs_close(&file);
        return (int)written;
    }

    fs_close(&file);
#else
    /* Software simulation fallback: print to debug console as a mock flash entry */
    LOG_INF("[SIMULATED FLASH WRITE] TS: %u | LAT: %.6f | LON: %.6f | ALT: %.2f | SPD: %.2f | R: %.2f | P: %.2f | Y: %.2f | SATS: %u | HDOP: %.2f",
            entry->timestamp, entry->latitude, entry->longitude,
            entry->altitude, entry->speed, entry->roll, entry->pitch,
            entry->yaw, entry->satellites, entry->hdop);
#endif

    return 0;
}

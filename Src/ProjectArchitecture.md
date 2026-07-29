thingy91x-gps-logger/
├── CMakeLists.txt
├── prj.conf
├── Kconfig
├── boards/
│
├── src/
│   ├── main.c
│   ├── app.c
│   └── app.h
│
├── modules/
│   ├── gps/
│   │   ├── gps.c
│   │   ├── gps.h
│   │   └── gps_parser.c
│   │
│   ├── storage/
│   │   ├── storage.c
│   │   ├── storage.h
│   │   ├── flash_fs.c
│   │   └── log_format.c
│   │
│   ├── sensors/
│   │   ├── sensors.c
│   │   ├── imu.c
│   │   ├── pressure.c
│   │   └── temperature.c
│   │
│   ├── power/
│   │   ├── power.c
│   │   └── battery.c
│   │
│   ├── communication/
│   │   ├── ble.c
│   │   ├── lte.c
│   │   ├── mqtt.c
│   │   └── usb.c
│   │
│   ├── logger/
│   │   ├── logger.c
│   │   └── logger.h
│   │
│   └── settings/
│       ├── settings.c
│       └── settings.h
│
├── include/
│
├── tests/
│
└── scripts/
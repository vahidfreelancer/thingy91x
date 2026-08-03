# Gemini Context: Nordic Thingy:91 X Application Framework

## 1. Project Overview & Architecture
This project is a modular, production-ready firmware platform based on the **Nordic Thingy:91 X** running **nRF Connect SDK (NCS) / Zephyr RTOS**.
The platform must support diverse deployment profiles (e.g., Asset Tracker, Environmental Sensor Node, Real-time Controller/Autopilot) via clean hardware abstractions and conditional subsystem initialization.

### Architecture Guidelines
- **Event-Driven Architecture:** Enforce the use of the **Application Event Manager (app_event_manager)** for inter-module communication. Modules must be fully decoupled; never cross-include application module headers directly.
- **Hardware Abstraction Layer (HAL):** Isolate hardware drivers behind generic driver interfaces (`/src/platform/drivers/`) to allow easy swapping of sensors or transport layers.
- **Dual-Core & Multi-SoC Protocol Split:**
  - `nRF9151` handles Cellular (LTE-M/NB-IoT), GNSS, and cloud protocols (MQTT/CoAP/LwM2M via nRF Cloud).
  - `nRF5340` (App/Net cores) handles short-range connectivity (BLE / Thread) and localized ultra-low-power sensing.
  - `nRF7002` handles Wi-Fi SSID scanning for location-based services (LBS).

## Project Structure & File Layout Directives

This project strictly follows the **nRF Connect SDK / Zephyr RTOS layout standard**. All generated code and file references must conform to these boundaries:

- **Root Build Configuration:**
  - `CMakeLists.txt` and `prj.conf` reside at the **project root**. Never reference `Src/CMakeLists.txt`.
  - Additional configuration overlays must be placed in `src/configs/` (e.g., `src/configs/production.conf`).

- **Hardware & Device Tree Adjustments (`boards/`):**
  - Do NOT modify vendor drivers or implement raw register board setups under `src/platform/`.
  - Board-specific hardware changes MUST be defined via Device Tree overlays in `boards/<board_target>.overlay`.

- **Application Logic (`src/app/`):**
  - High-level application modes (e.g., Asset Tracker, Autopilot, Environmental Monitor) live in dedicated modules inside `src/app/` (e.g., `src/app/asset_tracker`).
  - Active profile selection is done via the Kconfig option `CONFIG_APP_PROFILE_<NAME>`. The root `CMakeLists.txt` conditionally includes the respective application folder.
  - Modules must communicate asynchronously using `app_event_manager` or Zephyr system workqueues (`k_work`).

- **Services & Drivers (`src/services/`, `src/platform/drivers/`):**
  - `src/services/`: Modular subsystems (e.g., `telemetry`, `power`, `location`). Services must expose clean C APIs initialized via `SYS_INIT` or explicit module `init()` calls.
  - `src/platform/drivers/`: Sensor or peripheral abstractions that build on top of Zephyr `device.h` and `sensor.h` APIs.

- **Standard Library Policy (`src/libraries/`):**
  - **Do NOT generate custom implementations** for utilities already supplied by Zephyr (e.g., Ring buffers, CRC, Math macros, Slist/Dlist, Logging). 
  - Always use `<zephyr/sys/ring_buffer.h>`, `<zephyr/sys/crc.h>`, and `<zephyr/logging/log.h>`.

---

## 2. Resource Optimization & Constraints
Due to strict power budgets (for tracker profiles) and tight SRAM constraints on embedded targets:

- **Power Management (Critical):**
  - All worker threads must yield execution via Zephyr workqueues (`k_work`) or sleep states.
  - Never use blocking `k_busy_wait()` in production paths; use asynchronous timers (`k_timer`) or interrupts.
  - Power down unused peripherals explicitly (UARTs, I2C buses, Wi-Fi companion chip) when not actively sampling/transmitting.
  - Drivers and services must implement Zephyr's device power management interface (`CONFIG_PM_DEVICE`) to handle low-power states (suspended/resumed).
- **Dynamic Allocation Policy:**
  - Avoid `malloc()` / dynamic heap allocation after boot.
  - Use statically allocated Zephyr message queues (`k_msgq`), thread pools, or fixed-size memory slabs (`k_mem_slab`).
- **Network Data Minimization:**
  - Payload serialization should prefer binary formats (CBOR, Protobuf, or packed structs) over verbose JSON/Text for cellular uplinks. Use Zephyr's `zbor` library for lightweight CBOR serialization.

---

## 3. Real-Time Scheduling & Concurrency Policies
For safety-critical or control loop applications (like Autopilot):
- **Deterministic Scheduling**:
  - Time-critical control loops and high-rate sensor acquisition threads must run in **cooperative threads** (negative priority, e.g., `-1` to `-15`) to prevent preemption.
  - Telemetry transmission, logging, and general services must run in **preemptive threads** (positive priority, e.g., `1` to `15`).
- **Starvation Avoidance**:
  - Every cooperative thread must yield periodically (e.g., via `k_yield()` or `k_sleep()`) or block on a synchronization primitive (e.g., semaphore or message queue) to allow lower-priority threads execution time.
- **Thread Definition Guidelines**:
  - Define all application thread priorities and stack sizes centrally in `src/app/app_config.h` using Kconfig symbols to adjust per-profile needs.

---

## 4. Multi-SoC Communication & Boundaries
Thingy:91 X has two primary processing units:
- **Inter-SoC Boundaries**:
  - All cross-SoC communication (between `nRF9151` cellular host and `nRF5340` local sensor hub/BLE controller) must use a structured, asynchronous Remote Procedure Call (RPC) or message-passing protocol.
  - Avoid direct un-marshaled UART text parsing. Use structured binary formats (e.g., HDLC, COBS, or Protobuf-over-serial) with CRC verification.

---

## 5. Storage & Non-Volatile Memory (NVM) Policy
To maintain high reliability, write-efficiency, and wear leveling:
- **Configuration & State Storage**:
  - Small configurations, state variables, and settings must use Zephyr's **Settings Subsystem** (backed by Non-Volatile Storage - NVS).
- **Data Buffering & Telemetry Queuing**:
  - Large telemetry arrays or offline logs must be queued into a flash partition formatted with **LittleFS** via the Zephyr **Flash Map API**.
  - Direct flash raw writes are prohibited to prevent wear out of block sectors.

---

## 6. Code Style & Zephyr/NCS Coding Standards
- **Language Standard:** C11 for system firmware. C++ features are restricted unless explicitly required for external vendor libraries.
- **Naming Conventions:**
  - Module interfaces follow lowercase snake_case: `sensor_manager_init()`.
  - Config flags must strictly use Kconfig notation: `CONFIG_APP_*`.
  - Device Tree Nodes must be retrieved via standard Zephyr macros (`DT_NODELABEL`, `DEVICE_DT_GET`).
- **Error Handling:**
  - All functions returning status codes must return negative POSIX error codes (e.g., `-EINVAL`, `-EBUSY`, `-ETIMEDOUT`) on failure and `0` on success.
  - All hardware access function calls must validate return values and log failures via `LOG_ERR()`.

---

## 7. Configuration & Modular Build Rules
The project uses Kconfig overlays (`/boards/`) and Device Tree overlays (`.overlay`) to configure hardware applications without code modifications:

- **Profile Isolation:** Subsystems (e.g., GNSS, BLE mesh, Cellular, IMU processing) must be wrapped behind conditional Kconfig blocks (`#ifdef CONFIG_APP_FEATURE_XYZ`).
- **Device Tree:** Keep custom board overlay settings clean in `/boards/thingy91x_nrf9151_ns.overlay`.

---

## 8. Gemini Code Generation Directives
When generating code or proposing solutions:
1. Prioritize **Zephyr RTOS native APIs** (`k_thread`, `k_work`, `gpio_pin_configure_dt`) over generic standard library or raw register access.
2. Provide corresponding Kconfig symbol definitions (`Kconfig`) and Device Tree additions (`.overlay`) whenever introducing new driver integrations or features.
3. Always structure main application loops as cooperative tasks (Workqueues / State Machines) rather than infinite polling loops.
@echo off
setlocal

set ROOT=firmware-platform

echo Creating project structure...

:: Root
mkdir "%ROOT%"

:: ------------------------------------------------------------------
:: app
:: ------------------------------------------------------------------
mkdir "%ROOT%\app"

type nul > "%ROOT%\app\main.c"
type nul > "%ROOT%\app\app.c"
type nul > "%ROOT%\app\app.h"
type nul > "%ROOT%\app\app_config.h"

:: ------------------------------------------------------------------
:: platform
:: ------------------------------------------------------------------
mkdir "%ROOT%\platform"
mkdir "%ROOT%\platform\board"
mkdir "%ROOT%\platform\drivers"
mkdir "%ROOT%\platform\drivers\gnss"
mkdir "%ROOT%\platform\drivers\modem"
mkdir "%ROOT%\platform\drivers\imu"
mkdir "%ROOT%\platform\drivers\battery"
mkdir "%ROOT%\platform\drivers\flash"
mkdir "%ROOT%\platform\drivers\leds"
mkdir "%ROOT%\platform\drivers\buttons"
mkdir "%ROOT%\platform\os"
mkdir "%ROOT%\platform\utils"

:: ------------------------------------------------------------------
:: services
:: ------------------------------------------------------------------
mkdir "%ROOT%\services"
mkdir "%ROOT%\services\logging"
mkdir "%ROOT%\services\storage"
mkdir "%ROOT%\services\settings"
mkdir "%ROOT%\services\diagnostics"
mkdir "%ROOT%\services\telemetry"
mkdir "%ROOT%\services\location"
mkdir "%ROOT%\services\power"
mkdir "%ROOT%\services\scheduler"
mkdir "%ROOT%\services\events"
mkdir "%ROOT%\services\shell"

:: ------------------------------------------------------------------
:: libraries
:: ------------------------------------------------------------------
mkdir "%ROOT%\libraries"
mkdir "%ROOT%\libraries\ring_buffer"
mkdir "%ROOT%\libraries\crc"
mkdir "%ROOT%\libraries\filters"
mkdir "%ROOT%\libraries\math"
mkdir "%ROOT%\libraries\serialization"

:: ------------------------------------------------------------------
:: configs
:: ------------------------------------------------------------------
mkdir "%ROOT%\configs"

type nul > "%ROOT%\configs\development.conf"
type nul > "%ROOT%\configs\production.conf"
type nul > "%ROOT%\configs\testing.conf"

:: ------------------------------------------------------------------
:: tests
:: ------------------------------------------------------------------
mkdir "%ROOT%\tests"

:: ------------------------------------------------------------------
:: scripts
:: ------------------------------------------------------------------
mkdir "%ROOT%\scripts"

:: ------------------------------------------------------------------
:: docs
:: ------------------------------------------------------------------
mkdir "%ROOT%\docs"

:: ------------------------------------------------------------------
:: Root files
:: ------------------------------------------------------------------
type nul > "%ROOT%\CMakeLists.txt"
type nul > "%ROOT%\Kconfig"
type nul > "%ROOT%\prj.conf"
type nul > "%ROOT%\.gitignore"
type nul > "%ROOT%\README.md"

echo.
echo ========================================
echo Project created successfully.
echo Root folder: %ROOT%
echo ========================================
pause
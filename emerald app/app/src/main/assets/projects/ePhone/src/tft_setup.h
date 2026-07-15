/**
 * tft_setup.h — TFT_eSPI user setup for ESP32-2432S028 (CYD)
 *
 * IMPORTANT: This file is auto-included by TFT_eSPI.h via __has_include(<tft_setup.h>)
 * when placed in the sketch (src/) directory. No build_flags changes needed.
 *
 * Board:  ESP32-2432S028 (Elecrow / Cheap Yellow Display)
 * SoC:    ESP32-D0WD-V3 rev3.1
 * Flash:  4MB
 * USB:    CH340 (VID:PID=1A86:7523)
 * Port:   /dev/cu.usbserial-10
 *
 * ⚠️  BOARD REVISION NOTE (28.05.2026):
 *   This board uses HSPI (SPI2) with ST7789 display controller, NOT VSPI with ILI9341.
 *   See DIAGNOSTICS section in README.md for details.
 *
 * Pinout (this board):
 *   TFT_CS=15, TFT_DC=2, TFT_RST=4, TFT_BL=32
 *   TFT_MOSI=13 (HSPI), TFT_MISO=12 (HSPI), TFT_SCLK=14 (HSPI)
 *   TOUCH_CS=21 (XPT2046)
 */

// ============================================================
// Display Driver
// ============================================================
// ST7789 — confirmed via register read (ILI9341 IDs returned 0x10,0x81,0xB3)
#define ST7789_DRIVER
// #define ILI9341_DRIVER    // Old board only
// #define ILI9341_2_DRIVER  // Old board only

// ============================================================
// SPI Pin Definitions (ESP32 HSPI)
// ============================================================
// HSPI (SPI2) pins — this board revision routes display to HSPI
#define TFT_MISO   12
#define TFT_MOSI   13
#define TFT_SCLK   14
#define TFT_CS     15
#define TFT_DC      2
#define TFT_RST     4

// Backlight
#define TFT_BL     32
#define TFT_BACKLIGHT_ON HIGH

// Touch chip select (XPT2046)
#define TOUCH_CS   21

// ============================================================
// SPI Frequency (reduced for reliable communication)
// ============================================================
#define SPI_FREQUENCY        4000000    // 4 MHz
#define SPI_READ_FREQUENCY   2000000    // 2 MHz
#define SPI_TOUCH_FREQUENCY   2500000   // 2.5 MHz

// ============================================================
// Rotation & Colour Order
// ============================================================
#define TFT_ROTATION     0  // Portrait: USB left

// BGR colour order — required for this board revision
#define TFT_RGB_ORDER TFT_BGR

// Disable color inversion — this board revision shows inverted colors
// without this flag (BLACK → WHITE, RED → CYAN, etc.)
#define TFT_INVERSION_OFF

// ============================================================
// Fonts to load
// ============================================================
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// ============================================================
// ESP32 SPI port — HSPI (SPI2) instead of VSPI (SPI3)
// ============================================================
// This board revision routes the display to HSPI pins (MOSI=13, MISO=12, SCLK=14)
#define USE_HSPI_PORT

// ============================================================
// Auto-diagnostics marker
// ============================================================
#define TFT_SETUP_LOADED_FROM_SKETCH

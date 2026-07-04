/**
 * config.h — Emerald Configuration
 *
 * All user-configurable constants in one place.
 * Change OS name, user name, pins, colors, and timing here.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// OS Identity
// ============================================================
#define OS_NAME        "Emerald"
#define OS_VERSION     "v1.0.0"
#define OS_EDITION     "Developer Edition"
#define USER_NAME      "Fedir"
#define USER_LEVEL     9
#define USER_TITLE     "Junior Developer"
#define ACHIEVEMENT_COUNT 21

// ============================================================
// CYD Pin Definitions (real board: ST7789 on HSPI)
// ============================================================
#define PIN_TFT_BL     32      // Backlight PWM
#define PIN_TOUCH_CS   21      // XPT2046 chip select
#define PIN_TOUCH_IRQ  36      // XPT2046 IRQ (not used by default)

// RGB LED pins (common anode: LOW = ON)
// Actual board wiring: R=GPIO26, G=GPIO16, B=GPIO17
#define PIN_LED_R      26      // Red   — GPIO26 (shared with speaker)
#define PIN_LED_G      16      // Green — GPIO16
#define PIN_LED_B      17      // Blue  — GPIO17

// ============================================================
// Display dimensions (after rotation 1 = landscape)
// ============================================================
#define DISP_WIDTH      240
#define DISP_HEIGHT     320

// ============================================================
// Compile-time RGB888 → RGB565 converter (equivalent to tft.color565())
// ============================================================
#define TFT_COLOR(r,g,b) (((uint16_t)((r) & 0xF8) << 8) | ((uint16_t)((g) & 0xFC) << 3) | ((uint16_t)(b) >> 3))

// Color Scheme — dark green cyberpunk theme
// ============================================================
#define COLOR_BG        TFT_COLOR(6,10,7)         // very dark green-black background
#define COLOR_PANEL     TFT_COLOR(13,30,18)       // dark panel (card/header/statusbar bg)
#define COLOR_NEON      TFT_COLOR(57,217,122)     // main green neon  #39d97a
#define COLOR_NEON2     TFT_COLOR(168,224,99)     // lighter green accent  #a8e063
#define COLOR_TEXT      TFT_COLOR(208,240,216)    // light green text  #d0f0d8
#define COLOR_DIM       TFT_COLOR(58,106,74)      // muted green  #3a6a4a
#define COLOR_HIGHLIGHT TFT_COLOR(182,255,204)    // bright white-green for bold  #b6ffcc
#define COLOR_YELLOW    TFT_COLOR(255,225,0)      // accent yellow  #ffe100
#define COLOR_RED       TFT_COLOR(220,40,40)      // red for errors
#define COLOR_BLACK     TFT_COLOR(0,0,0)
#define COLOR_WHITE     TFT_COLOR(255,255,255)

// Deep purple background
#define COLOR_PURPLE    TFT_COLOR(30, 10, 50)      // deep dark purple
#define COLOR_PURPLE_PANEL  TFT_COLOR(50, 20, 80)  // slightly lighter purple
#define COLOR_PURPLE_NEON   TFT_COLOR(160, 80, 255) // neon purple

// ============================================================
// Theme / Background selection
// ============================================================
enum BackgroundTheme : uint8_t {
    BG_EMERALD  = 0,   // Default dark green cyberpunk
    BG_BLACK    = 1,   // Pure black
    BG_PURPLE   = 2    // Deep purple
};

// Global theme setting (can be changed at runtime)
extern BackgroundTheme g_backgroundTheme;

// ============================================================
// Boot Timing (milliseconds)
// ============================================================
#define BOOT_LINE_DELAY    350    // delay between boot lines
#define BOOT_TOTAL_TIME    4500   // total boot sequence duration
#define STORY_AUTO_NEXT    0      // 0 = only tap, >0 = auto advance ms

// ============================================================
// Touch Calibration (XPT2046 raw -> screen coords)
// ============================================================
#define TOUCH_X_MIN    200
#define TOUCH_X_MAX    3700
#define TOUCH_Y_MIN    240
#define TOUCH_Y_MAX    3800
#define TOUCH_SWAP_XY  true
#define TOUCH_INVERT_X false
#define TOUCH_INVERT_Y true

// Touch filter
#define TOUCH_Z_MIN    200
#define TOUCH_Z_MAX    3500
#define TOUCH_DEBOUNCE_MS 200

// ============================================================
// UI Layout Constants
// ============================================================
#define STATUSBAR_HEIGHT   20
#define HEADER_HEIGHT      28
#define BACK_BUTTON_W      48
#define BACK_BUTTON_H      28
#define CARD_RADIUS         4

// ============================================================
// Animation Timing
// ============================================================
#define FADE_DURATION_MS   300
#define TYPEWRITER_CHAR_MS  30
#define SLIDE_DURATION_MS  400

#endif // CONFIG_H

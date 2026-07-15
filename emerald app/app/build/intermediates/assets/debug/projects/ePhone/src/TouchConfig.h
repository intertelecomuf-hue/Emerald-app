/**
 * TouchConfig.h — Touch calibration settings for XPT2046
 *
 * Згенеровано автоматично після калібрування.
 * TOUCH_CALIBRATION_ENABLED = false пропускає калібрувальний екран.
 */

#ifndef TOUCH_CONFIG_H
#define TOUCH_CONFIG_H

// ============================================================
// Master switch: false = skip calibration screen at boot
// ============================================================
#define TOUCH_CALIBRATION_ENABLED true

// ============================================================
// Raw 12-bit ADC min/max from XPT2046 (0-4095)
// ============================================================
#define TOUCH_RAW_MIN_X   200
#define TOUCH_RAW_MAX_X   3700
#define TOUCH_RAW_MIN_Y   240
#define TOUCH_RAW_MAX_Y   3800

// ============================================================
// Axis transformations
// ============================================================
#define TOUCH_SWAP_XY     true   // swap X and Y axes
#define TOUCH_INVERT_X    false  // invert X axis (4095 - x)
#define TOUCH_INVERT_Y    true   // invert Y axis (4095 - y)

// ============================================================
// Display resolution (pixels)
// ============================================================
#define TOUCH_SCREEN_W    320
#define TOUCH_SCREEN_H    240

#endif // TOUCH_CONFIG_H

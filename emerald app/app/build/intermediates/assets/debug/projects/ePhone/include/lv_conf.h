/**
 * @file lv_conf.h
 * Configuration file for LVGL 9.x
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/* Enable LVGL */
#define LV_USE_LVGL 1

/*-------------------------
 * Color settings
 *-------------------------*/
#define LV_COLOR_DEPTH 16                    /* 16-bit color (RGB565) for ILI9341 */
#define LV_COLOR_16_SWAP 0                   /* Swap bytes for RGB565 */

/*-------------------------
 * Memory settings
 *-------------------------*/
#define LV_MEM_CUSTOM 1                      /* Use custom memory allocation */
#if LV_MEM_CUSTOM == 0
    #define LV_MEM_SIZE (48U * 1024U)         /* 48 KB heap */
    #define LV_MEM_POOL_INCLUDE
    #define LV_MEM_POOL_ALLOC
#endif

/*-------------------------
 * HAL settings
 *-------------------------*/
#define LV_TICK_CUSTOM 1                     /* Custom tick source */
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*-------------------------
 * Display buffer size
 *-------------------------*/
#define LV_DISP_DEF_REFR_PERIOD 30           /* 30 ms refresh */
#define LV_DPI_DEF 130                       /* DPI of the display */

/*-------------------------
 * Features
 *-------------------------*/
#define LV_USE_LOG 0                         /* Disable logging (saves flash) */
#define LV_USE_ASSERT_NULL 0
#define LV_USE_ASSERT_MALLOC 0
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_ASSERT_MEM_INTEGRITY 0

/* Draw */
#define LV_USE_DRAW_SW 1                     /* Software rendering */
#define LV_USE_DRAW_VGLITE 0
#define LV_USE_DRAW_OPAI 0

/* Widgets */
#define LV_USE_ANIMIMG 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BUTTON 1
#define LV_USE_BUTTONMATRIX 1
#define LV_USE_CALENDAR 1
#define LV_USE_CANVAS 1
#define LV_USE_CHART 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMAGE 1
#define LV_USE_IMAGEBUTTON 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LABEL 1
#define LV_USE_LED 1
#define LV_USE_LINE 1
#define LV_USE_LIST 1
#define LV_USE_MENU 1
#define LV_USE_MSGBOX 1
#define LV_USE_ROLLER 1
#define LV_USE_SCALE 1
#define LV_USE_SLIDER 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_SWITCH 1
#define LV_USE_TABLE 1
#define LV_USE_TABVIEW 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1

/* Extra widgets */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*-------------------------
 * Fonts
 *-------------------------*/
#define LV_FONT_MONTSERRAT_8  1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_48 1

/* Symbols (for icons like Wi-Fi, Bluetooth, etc.) */
#define LV_FONT_UNSCII_8 1

/*-------------------------
 * Miscellaneous
 *-------------------------*/
#define LV_BUILD_EXAMPLES 0                  /* Disable examples (saves space) */

/* Use LVGL 9.x internal API */
#define LV_CONF_SUPPRESS_DEPRECATED_WARNINGS 0

#endif /* LV_CONF_H */

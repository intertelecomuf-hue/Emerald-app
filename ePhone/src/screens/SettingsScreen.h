/**
 * SettingsScreen.h — Settings: background theme selection
 *
 * Allows changing the device wallpaper/background:
 *   - Emerald  (default dark green)
 *   - Black    (pure black)
 *   - Purple   (deep purple)
 */
#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "../config.h"

class SettingsScreen {
public:
    SettingsScreen();

    void start();
    bool update();
    void handleTap(int x, int y);
    bool isCalibrateRequested() const;
    void clearCalibrateRequest();

private:
    struct ThemeOption {
        const char* label;
        BackgroundTheme theme;
        uint16_t previewColor;
    };

    static const ThemeOption _options[3];
    int _selectedOption;
    bool _changed;

    void draw();
    void drawOption(int index, bool selected);
    void drawCalibrateButton();

    // Calibrate button coordinates
    static const int CALIB_Y = HEADER_HEIGHT + 36 + 3 * 48 + 8;
    static const int CALIB_H = 36;
};

#endif // SETTINGS_SCREEN_H

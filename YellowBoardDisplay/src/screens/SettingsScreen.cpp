/**
 * SettingsScreen.cpp — Background theme selection
 */
#include "SettingsScreen.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

const SettingsScreen::ThemeOption SettingsScreen::_options[3] = {
    { "Emerald (green)",  BG_EMERALD, TFT_COLOR(6,10,7) },
    { "Black",            BG_BLACK,   TFT_COLOR(0,0,0) },
    { "Deep Purple",      BG_PURPLE,  TFT_COLOR(30,10,50) },
};

SettingsScreen::SettingsScreen()
    : _selectedOption(0)
    , _changed(false)
{
}

void SettingsScreen::start() {
    _selectedOption = g_backgroundTheme;
    _changed = false;
    draw();
}

bool SettingsScreen::update() {
    return false;
}

void SettingsScreen::draw() {
    tft.fillScreen(COLOR_BLACK);
    drawHeader("SETTINGS", true);

    tft.setTextColor(COLOR_NEON, COLOR_BLACK);
    tft.setTextSize(1);
    tft.drawString("Background theme:", 10, HEADER_HEIGHT + 10, 2);

    for (int i = 0; i < 3; i++) {
        drawOption(i, (i == _selectedOption));
    }

    drawCalibrateButton();

    drawStatusBarType("TAP TO SELECT", COLOR_YELLOW);
}

void SettingsScreen::drawCalibrateButton() {
    int x = 10;
    int w = DISP_WIDTH - 20;
    int y = CALIB_Y;

    tft.fillRoundRect(x, y, w, CALIB_H, CARD_RADIUS, TFT_COLOR(40, 40, 60));
    tft.drawRoundRect(x, y, w, CALIB_H, CARD_RADIUS, COLOR_DIM);

    tft.setTextColor(TFT_COLOR(160, 160, 255), TFT_COLOR(40, 40, 60));
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(">> Calibrate Touch <<", x + 12, y + CALIB_H / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

void SettingsScreen::drawOption(int index, bool selected) {
    int y = HEADER_HEIGHT + 36 + index * 48;
    int x = 10;
    int w = DISP_WIDTH - 20;
    int h = 40;

    uint16_t bg = selected ? COLOR_PANEL : COLOR_BLACK;
    uint16_t border = selected ? COLOR_NEON : COLOR_DIM;

    tft.fillRoundRect(x, y, w, h, CARD_RADIUS, bg);
    tft.drawRoundRect(x, y, w, h, CARD_RADIUS, border);

    // Color preview square
    uint16_t preview = _options[index].previewColor;
    tft.fillRect(x + 8, y + 8, 24, 24, preview);
    tft.drawRect(x + 8, y + 8, 24, 24, COLOR_DIM);

    // Label
    tft.setTextColor(selected ? COLOR_NEON : COLOR_TEXT, bg);
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(_options[index].label, x + 42, y + h / 2, 2);
    tft.setTextDatum(TL_DATUM);

    // Checkmark if selected
    if (selected) {
        tft.setTextColor(COLOR_NEON, bg);
        tft.setTextSize(1);
        tft.setTextDatum(MR_DATUM);
        tft.drawString("✓", x + w - 10, y + h / 2, 2);
        tft.setTextDatum(TL_DATUM);
    }
}

bool SettingsScreen::isCalibrateRequested() const {
    return _changed;  // reuse _changed as calibrate flag when button area tapped
}

void SettingsScreen::clearCalibrateRequest() {
    _changed = false;
}

void SettingsScreen::handleTap(int x, int y) {
    // Back button pressed
    if (y < HEADER_HEIGHT) {
        if (isBackButtonPressed(x, y)) {
            // If theme changed, mark it
            if (g_backgroundTheme != (BackgroundTheme)_selectedOption) {
                g_backgroundTheme = (BackgroundTheme)_selectedOption;
                _changed = true;
            }
            // Return to menu (handled by main.cpp transition)
        }
        return;
    }

    // Check if Calibrate Touch button was pressed
    if (y >= CALIB_Y && y <= CALIB_Y + CALIB_H) {
        // Signal to main.cpp that calibrate was requested
        _changed = true;  // reuse as "goto calibrate" flag
        return;  // main.cpp will detect and transition
    }

    // Calculate which option was tapped
    int startY = HEADER_HEIGHT + 36;
    int idx = (y - startY) / 48;
    if (idx >= 0 && idx < 3) {
        _selectedOption = idx;
        g_backgroundTheme = (BackgroundTheme)idx;
        _changed = true;
        draw();
        Serial.printf("[Settings] Theme changed to: %d (%s)\n",
                      idx, _options[idx].label);
    }
}

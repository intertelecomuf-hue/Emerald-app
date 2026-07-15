/**
 * MenuScreen.cpp — Single Wi-Fi tile screen
 */

#include "MenuScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

const MenuScreen::MenuTile MenuScreen::_tiles[MENU_TILE_COUNT] = {
    { "[WIF]", "Wi-Fi" },
};

MenuScreen::MenuScreen()
    : _selectedTile(-1)
    , _highlightEnd(0)
{
}

void MenuScreen::start() {
    _selectedTile = -1;
    _highlightEnd = 0;

    tft.fillScreen(COLOR_BG);
    drawHeader();
    drawTile(0, false);
    drawStatusBarInternal();
}

bool MenuScreen::update() {
    if (_highlightEnd > 0 && millis() >= _highlightEnd) {
        if (_selectedTile >= 0 && _selectedTile < MENU_TILE_COUNT) {
            drawTile(_selectedTile, false);
        }
        _selectedTile = -1;
        _highlightEnd = 0;
    }
    return false;
}

int MenuScreen::handleTap(int x, int y) {
    if (y < 32) return -1;
    if (y >= DISP_HEIGHT - STATUSBAR_HEIGHT) return -1;

    int idx = hitTest(x, y);
    if (idx >= 0 && idx < MENU_TILE_COUNT) {
        _selectedTile = idx;
        _highlightEnd = millis() + 150;
        drawTile(idx, true);
        return idx;
    }
    return -1;
}

int MenuScreen::selectedTile() {
    return _selectedTile;
}

int MenuScreen::hitTest(int x, int y) {
    // Tile position (centered)
    int tx = (DISP_WIDTH - TILE_W) / 2;   // X: 20
    int ty = (DISP_HEIGHT - STATUSBAR_HEIGHT - TILE_H) / 2;  // Y: 90
    
    // Center point (invisible)
    int cx = tx + TILE_W / 2;  // 120
    int cy = ty + TILE_H / 2;  // 150
    
    // Hit area = full tile contour + tolerance
    const int TOL = 15;
    int left   = tx - TOL;     // 5
    int right  = tx + TILE_W + TOL;  // 235
    int top    = ty - TOL;     // 75
    int bottom = ty + TILE_H + TOL;  // 225
    
    Serial.printf("[WiFi] Center=(%d,%d) Area=[%d..%d, %d..%d] Tap=(%d,%d)\n",
                  cx, cy, left, right, top, bottom, x, y);
    
    if (x >= left && x <= right && y >= top && y <= bottom) {
        return 0;
    }
    return -1;
}

void MenuScreen::drawTile(int index, bool highlight) {
    (void)index;
    int tx = (DISP_WIDTH - TILE_W) / 2;
    int ty = (DISP_HEIGHT - STATUSBAR_HEIGHT - TILE_H) / 2;

    uint16_t borderColor = highlight ? COLOR_NEON : COLOR_DIM;
    uint16_t bgColor = highlight ? COLOR_DIM : COLOR_PANEL;

    // Clear area
    tft.fillRect(tx - 2, ty - 2, TILE_W + 4, TILE_H + 4, COLOR_BG);

    // Card
    tft.fillRoundRect(tx, ty, TILE_W, TILE_H, CARD_RADIUS, bgColor);
    tft.drawRoundRect(tx, ty, TILE_W, TILE_H, CARD_RADIUS, borderColor);

    // Wi-Fi icon
    tft.setTextColor(COLOR_NEON, bgColor);
    tft.setTextSize(2);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("[WIF]", tx + TILE_W / 2, ty + 35, 4);
    tft.setTextDatum(TL_DATUM);

    // Title
    tft.setTextColor(COLOR_NEON, bgColor);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("Wi-Fi", tx + TILE_W / 2, ty + 75, 2);
    tft.setTextDatum(TL_DATUM);
}

void MenuScreen::drawHeader() {
    char header[48];
    snprintf(header, sizeof(header), "%s    %s LVL%d",
             OS_NAME, USER_NAME, USER_LEVEL);

    tft.fillRect(0, 0, DISP_WIDTH, HEADER_HEIGHT, COLOR_PANEL);
    tft.drawFastHLine(0, HEADER_HEIGHT, DISP_WIDTH, COLOR_DIM);

    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(header, DISP_WIDTH / 2, HEADER_HEIGHT / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

void MenuScreen::drawStatusBarInternal() {
    drawStatusBar("ONLINE", "09:41");
}

void MenuScreen::highlightTile(int index) {
    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        drawTile(i, i == index);
    }
    _selectedTile = index;
    _highlightEnd = millis() + 2000;
}

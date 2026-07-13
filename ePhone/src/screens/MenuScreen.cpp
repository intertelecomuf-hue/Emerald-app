/**
 * MenuScreen.cpp — Main Menu (2×4 grid) with Emerald background
 */

#include "MenuScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"
#include "../ui/Animations.h"

extern TFT_eSPI tft;

// ============================================================
// Menu Tile Definitions
// ============================================================
const MenuScreen::MenuTile MenuScreen::_tiles[MENU_TILE_COUNT] = {
    { "[USR]", "Мій Профіль",     "Рівень, досягнення"    },   // 0
    { "[MAP]", "Мої Місії",       "Повна траєкторія"      },   // 1
    { "[WIF]", "Wi-Fi",           "WiFi + Bluetooth"       },   // 2
    { "[SET]", "Налаштування",    "Тло, тема"              },   // 3
    { "[GAM]", "Міні Ігри",       "Snake, Space Shooter"   },   // 4
    { "[TMR]", "Системна Консоль","Термінал, команди"     },   // 5
    { "[MSG]", "Від Родини",      "Повідомлення від батьків"}, // 6
};

// ============================================================
MenuScreen::MenuScreen()
    : _selectedTile(-1)
    , _highlightEnd(0)
{
}

/** Draw a bright Emerald crystal watermark in the background */
static void drawEmeraldBackground() {
    int cx = DISP_WIDTH / 2;   // 160
    int cy = DISP_HEIGHT / 2 + 6;  // 126

    // Outer glow — brighter
    for (int r = 60; r >= 45; r--) {
        uint8_t brightness = map(r, 45, 60, 0, 30);
        tft.drawCircle(cx, cy, r, TFT_COLOR(0, brightness, brightness/4));
    }

    // Crystal shape — bright green
    // Left facets (darker)
    tft.fillTriangle(cx, cy-40, cx-25, cy-20, cx-35, cy,       TFT_COLOR(20, 80, 40));
    tft.fillTriangle(cx, cy-40, cx-35, cy,   cx-25, cy+20,     TFT_COLOR(15, 60, 30));
    tft.fillTriangle(cx-25, cy+20, cx-35, cy, cx, cy+40,       TFT_COLOR(10, 40, 20));

    // Right facets (brighter — light source)
    tft.fillTriangle(cx, cy-40, cx+25, cy-20, cx+35, cy,       TFT_COLOR(40, 170, 85));
    tft.fillTriangle(cx, cy-40, cx+35, cy,   cx+25, cy+20,     TFT_COLOR(50, 190, 95));
    tft.fillTriangle(cx+25, cy+20, cx+35, cy, cx, cy+40,       TFT_COLOR(30, 130, 65));

    // Center facet (highlight)
    tft.fillTriangle(cx, cy, cx-18, cy-12, cx+18, cy-12,       TFT_COLOR(60, 220, 110));
    tft.fillTriangle(cx, cy, cx-18, cy+12, cx+18, cy+12,       TFT_COLOR(45, 180, 90));

    // Bright sparkle
    tft.fillTriangle(cx+5, cy-32, cx+18, cy-24, cx+10, cy-20,  TFT_COLOR(100, 255, 140));

    // Borders
    tft.drawLine(cx, cy-40, cx+25, cy-20, TFT_COLOR(80, 220, 110));
    tft.drawLine(cx, cy-40, cx-25, cy-20, TFT_COLOR(50, 160, 80));
    tft.drawLine(cx+25, cy-20, cx+35, cy, TFT_COLOR(80, 220, 110));
    tft.drawLine(cx-25, cy-20, cx-35, cy, TFT_COLOR(50, 160, 80));
    tft.drawLine(cx+35, cy, cx+25, cy+20, TFT_COLOR(80, 220, 110));
    tft.drawLine(cx-35, cy, cx-25, cy+20, TFT_COLOR(50, 160, 80));
    tft.drawLine(cx+25, cy+20, cx, cy+40, TFT_COLOR(80, 220, 110));
    tft.drawLine(cx-25, cy+20, cx, cy+40, TFT_COLOR(50, 160, 80));

    // Center lines
    tft.drawLine(cx-33, cy, cx+33, cy, TFT_COLOR(30, 120, 60));
    tft.drawLine(cx, cy-38, cx, cy, TFT_COLOR(30, 120, 60));

    // "EMERALD" text — big and bright, below the crystal
    tft.setTextColor(TFT_COLOR(40, 180, 90), COLOR_BG);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("EMERALD", cx, cy + 58, 2);
    tft.setTextDatum(TL_DATUM);
}

void MenuScreen::start() {
    _selectedTile = -1;
    _highlightEnd = 0;

    tft.fillScreen(COLOR_BG);
    drawEmeraldBackground();
    drawHeader();
    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        drawTile(i, false);
    }
    drawStatusBarInternal();
}

bool MenuScreen::update() {
    // If there's an active highlight, check if it's expired
    if (_highlightEnd > 0 && millis() >= _highlightEnd) {
        // Redraw the tile without highlight
        if (_selectedTile >= 0 && _selectedTile < MENU_TILE_COUNT) {
            drawTile(_selectedTile, false);
        }
        _selectedTile = -1;
        _highlightEnd = 0;
    }
    return false;
}

int MenuScreen::handleTap(int x, int y) {
    Serial.printf("[MENU] handleTap(%d,%d) GRID_Y=%d STATUSBAR_Y=%d\n",
                  x, y, GRID_Y, DISP_HEIGHT - STATUSBAR_HEIGHT);

    // Header area? Ignore
    if (y < GRID_Y) { Serial.println("[MENU] -> header area, ignored"); return -1; }

    // Status bar area? Ignore
    if (y >= DISP_HEIGHT - STATUSBAR_HEIGHT) { Serial.println("[MENU] -> status bar, ignored"); return -1; }

    int idx = hitTest(x, y);
    Serial.printf("[MENU] hitTest(%d,%d) = %d\n", x, y, idx);
    if (idx >= 0 && idx < MENU_TILE_COUNT) {
        _selectedTile = idx;
        _highlightEnd = millis() + 150; // 150ms highlight
        drawTile(idx, true);

        // Return the tile index after a short delay
        // (caller will handle the transition)
        return idx;
    }

    return -1;
}

int MenuScreen::selectedTile() {
    return _selectedTile;
}

int MenuScreen::hitTest(int x, int y) {
    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        int col = i % 2;
        int row = i / 2;
        int tx = GRID_X + col * (TILE_W + TILE_GAP);
        int ty = GRID_Y + row * (TILE_H + TILE_GAP);

        // Last tile (row 3) centered
        if (i == 6) {
            tx = (DISP_WIDTH - TILE_W) / 2;
            ty = GRID_Y + 3 * (TILE_H + TILE_GAP);
        }

        Serial.printf("[MENU] tile[%d] rect=(%d,%d)-(%d,%d)  tap=(%d,%d) %s\n",
                      i, tx, ty, tx+TILE_W, ty+TILE_H, x, y,
                      (x >= tx && x <= tx + TILE_W && y >= ty && y <= ty + TILE_H) ? "HIT" : "");

        if (x >= tx && x <= tx + TILE_W && y >= ty && y <= ty + TILE_H) {
            return i;
        }
    }
    return -1;
}

void MenuScreen::drawTile(int index, bool highlight) {
    int col = index % 2;
    int row = index / 2;
    int tx = GRID_X + col * (TILE_W + TILE_GAP);
    int ty = GRID_Y + row * (TILE_H + TILE_GAP);

    // Last tile centered
    if (index == 6) {
        tx = (DISP_WIDTH - TILE_W) / 2;
        ty = GRID_Y + 3 * (TILE_H + TILE_GAP);
    }

    uint16_t borderColor = highlight ? COLOR_NEON : COLOR_DIM;
    uint16_t bgColor = highlight ? COLOR_DIM : COLOR_PANEL;

    // Card
    tft.fillRoundRect(tx, ty, TILE_W, TILE_H, CARD_RADIUS, bgColor);
    tft.drawRoundRect(tx, ty, TILE_W, TILE_H, CARD_RADIUS, borderColor);

    const MenuTile &t = _tiles[index];

    // Icon — centered in upper portion (font 4)
    tft.setTextColor(COLOR_NEON, bgColor);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(t.icon, tx + TILE_W / 2, ty + 18, 4);
    tft.setTextDatum(TL_DATUM);

    // Title — centered in lower portion (font 2)
    tft.setTextColor(COLOR_NEON, bgColor);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(t.title, tx + TILE_W / 2, ty + 42, 2);
    tft.setTextDatum(TL_DATUM);

    // No subtitle — removed per user request (doesn't fit 44px tile)
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
    // Redraw all tiles without highlight
    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        drawTile(i, false);
    }
    // Draw the selected tile with highlight
    if (index >= 0 && index < MENU_TILE_COUNT) {
        drawTile(index, true);
    }
    _selectedTile = index;
    _highlightEnd = millis() + 2000;
}

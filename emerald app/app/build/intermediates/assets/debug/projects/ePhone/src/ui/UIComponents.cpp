/**
 * UIComponents.cpp — Reusable UI drawing primitives
 */

#include "UIComponents.h"
#include "../config.h"
#include "../services/WiFiService.h"

// ============================================================
// Back Button
// ============================================================
void drawBackButton() {
    int x = 4, y = 2;
    int w = BACK_BUTTON_W, h = BACK_BUTTON_H;

    // Rounded rectangle
    tft.fillRoundRect(x, y, w, h, CARD_RADIUS, COLOR_PANEL);
    tft.drawRoundRect(x, y, w, h, CARD_RADIUS, COLOR_DIM);

    // Arrow "<-" text
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CL_DATUM);
    tft.drawString("<-", x + 10, y + h / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

void drawBackButtonHighlighted(bool highlight) {
    int x = 4, y = 2;
    int w = BACK_BUTTON_W, h = BACK_BUTTON_H;

    if (highlight) {
        // Highlighted state — bright border and background
        tft.fillRoundRect(x, y, w, h, CARD_RADIUS, COLOR_NEON);
        tft.drawRoundRect(x, y, w, h, CARD_RADIUS, COLOR_HIGHLIGHT);
        tft.setTextColor(COLOR_BLACK, COLOR_NEON);
    } else {
        // Normal state
        tft.fillRoundRect(x, y, w, h, CARD_RADIUS, COLOR_PANEL);
        tft.drawRoundRect(x, y, w, h, CARD_RADIUS, COLOR_DIM);
        tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    }

    tft.setTextSize(1);
    tft.setTextDatum(CL_DATUM);
    tft.drawString("<-", x + 10, y + h / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

bool isBackButtonPressed(int tx, int ty) {
    int x = 4, y = 2;
    return (tx >= x && tx <= x + BACK_BUTTON_W &&
            ty >= y && ty <= y + BACK_BUTTON_H);
}

// ============================================================
// Card
// ============================================================
void drawCard(int x, int y, int w, int h, uint16_t borderColor) {
    tft.fillRoundRect(x, y, w, h, CARD_RADIUS, COLOR_PANEL);
    tft.drawRoundRect(x, y, w, h, CARD_RADIUS, borderColor);
}

// ============================================================
// Progress Bar
// ============================================================
void drawProgressBar(int x, int y, int w, int h,
                     int percent, uint16_t color) {
    // Background
    tft.fillRoundRect(x, y, w, h, 3, COLOR_DIM);

    // Fill
    int fillW = (percent * w) / 100;
    if (fillW > 0) {
        tft.fillRoundRect(x, y, fillW, h, 3, color);
    }

    // Border
    tft.drawRoundRect(x, y, w, h, 3, COLOR_NEON);

    // Percentage text on the right
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(buf, x + w + 6, y + h / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Status Bar
// ============================================================
void drawStatusBar(const char* status, const char* timeStr) {
    int y = DISP_HEIGHT - STATUSBAR_HEIGHT;

    // Background
    tft.fillRect(0, y, DISP_WIDTH, STATUSBAR_HEIGHT, COLOR_PANEL);
    tft.drawFastHLine(0, y, DISP_WIDTH, COLOR_DIM);

    // Check WiFi status — show global connection info if connected
    WiFiService& wifi = WiFiService::instance();
    if (wifi.isConnected()) {
        // Use WiFi status bar instead of generic one
        drawWiFiStatusBar(wifi.getSSID(), wifi.getSignalBars(),
                          wifi.isDataActive(), timeStr);
        return;
    }

    // Left: connection status (● ONLINE)
    tft.setTextColor(COLOR_NEON2, COLOR_PANEL);
    tft.setTextSize(1);
    tft.drawString(status, 6, y + 4, 2);

    // Right: time
    tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
    tft.setTextDatum(CR_DATUM);
    tft.drawString(timeStr, DISP_WIDTH - 6, y + 4, 2);
    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Status Bar with custom color (for password entry, connecting)
// ============================================================
void drawStatusBarType(const char* status, uint16_t color) {
    int y = DISP_HEIGHT - STATUSBAR_HEIGHT;

    tft.fillRect(0, y, DISP_WIDTH, STATUSBAR_HEIGHT, COLOR_PANEL);
    tft.drawFastHLine(0, y, DISP_WIDTH, COLOR_DIM);

    tft.setTextColor(color, COLOR_PANEL);
    tft.setTextSize(1);
    tft.drawString(status, 6, y + 4, 2);
}

// ============================================================
// WiFi Status Bar — signal bars + SSID + data activity
// ============================================================
void drawWiFiStatusBar(const char* ssid, int signalBars,
                       bool dataActive, const char* timeStr) {
    int y = DISP_HEIGHT - STATUSBAR_HEIGHT;

    // Background
    tft.fillRect(0, y, DISP_WIDTH, STATUSBAR_HEIGHT, COLOR_PANEL);
    tft.drawFastHLine(0, y, DISP_WIDTH, COLOR_DIM);

    // Signal bars
    if (signalBars > 0) {
        drawSignalBars(6, y + 2, signalBars);
    }

    // SSID text (after bars: bars are ~30px wide)
    int textX = signalBars > 0 ? 40 : 6;
    tft.setTextColor(COLOR_NEON2, COLOR_PANEL);
    tft.setTextSize(1);
    char buf[24];
    if (ssid && strlen(ssid) > 0) {
        snprintf(buf, sizeof(buf), "%s", ssid);
    } else {
        snprintf(buf, sizeof(buf), "Wi-Fi");
    }
    tft.drawString(buf, textX, y + 4, 1);

    // Data activity indicator (RX/TX arrows)
    if (dataActive) {
        // Blinking TX/RX arrows
        tft.setTextColor(COLOR_NEON, COLOR_PANEL);
        tft.setTextSize(1);
        tft.setTextDatum(CC_DATUM);
        tft.drawString("⬆⬇", DISP_WIDTH - 40, y + 10, 1);
        tft.setTextDatum(TL_DATUM);
    } else {
        // Clear the area where arrows were
        tft.fillRect(DISP_WIDTH - 50, y + 2, 20, STATUSBAR_HEIGHT - 4, COLOR_PANEL);
    }

    // Time on far right
    tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CR_DATUM);
    tft.drawString(timeStr, DISP_WIDTH - 6, y + 4, 2);
    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Dot Indicators
// ============================================================
void drawDots(int total, int current, int y) {
    int spacing = 14;
    int startX = (DISP_WIDTH - (total * spacing)) / 2 + spacing / 2;

    for (int i = 0; i < total; i++) {
        int x = startX + i * spacing;
        uint16_t color = (i <= current) ? COLOR_NEON : COLOR_DIM;
        tft.fillCircle(x, y, 3, color);
    }
}

// ============================================================
// Signal Bars
// ============================================================
void drawSignalBars(int x, int y, int strength) {
    // strength: 0-4 (0 = no signal)
    int barW = 5;
    int gap = 2;
    int heights[] = { 4, 8, 12, 16 };
    uint16_t colors[] = { COLOR_RED, COLOR_YELLOW, COLOR_NEON2, COLOR_NEON };

    for (int i = 0; i < 4; i++) {
        int h = heights[i];
        int bx = x + i * (barW + gap);
        int by = y + (16 - h);
        uint16_t c = (i < strength) ? colors[i] : COLOR_DIM;
        tft.fillRect(bx, by, barW, h, c);
    }
}

// ============================================================
// Header
// ============================================================
void drawHeader(const char* title, bool showBack) {
    tft.fillRect(0, 0, DISP_WIDTH, HEADER_HEIGHT, COLOR_PANEL);
    tft.drawFastHLine(0, HEADER_HEIGHT, DISP_WIDTH, COLOR_DIM);

    if (showBack) {
        drawBackButton();
    }

    // Title centered
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(title, DISP_WIDTH / 2, HEADER_HEIGHT / 2, 2);
    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Tile Highlight (for menu press feedback)
// ============================================================
void highlightTile(int x, int y, int w, int h) {
    // Invert border to NEON for 150ms feedback
    tft.drawRoundRect(x, y, w, h, CARD_RADIUS, COLOR_NEON);
    tft.fillRect(x + 2, y + 2, w - 4, h - 4, COLOR_DIM);
    // Restore after short delay is handled by the caller redrawing
}

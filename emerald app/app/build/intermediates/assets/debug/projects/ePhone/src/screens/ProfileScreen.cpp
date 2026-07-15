/**
 * ProfileScreen.cpp — [USR] My Profile
 *
 * Avatar circle "F", name, level, 2x2 stat boxes, badge pills, back button.
 */

#include "ProfileScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

// ============================================================
// Badge definitions
// ============================================================
static const char* BADGES[] = {
    "IoT Піонер",   "Інж. Консолі",
    "Space Dev",    "BT Експерт",
    "Web Dev",      "Kahoot Чемпіон",
    "Майстер Паяння",
};

static const int BADGE_COUNT = 7;

ProfileScreen::ProfileScreen() {}

void ProfileScreen::start() {
    tft.fillScreen(COLOR_BG);
    drawHeader("[ МІЙ ПРОФІЛЬ ]", false);

    drawContent();
    // No status bar — back button at bottom instead
}

bool ProfileScreen::update() { return false; }

void ProfileScreen::handleTap(int x, int y) {
    (void)x; (void)y;
}

// ============================================================
// Draw a single stat box (rounded rect with dim border)
// ============================================================
void ProfileScreen::drawStatBox(int x, int y, int w, int h,
                                 const char* label, const char* value) {
    tft.fillRoundRect(x, y, w, h, 4, COLOR_PANEL);
    tft.drawRoundRect(x, y, w, h, 4, COLOR_DIM);

    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_DIM, COLOR_PANEL);
    tft.drawString(label, x + 5, y + 4, 1);

    tft.setTextSize(2);
    tft.setTextColor(COLOR_YELLOW, COLOR_PANEL);
    tft.drawString(value, x + 5, y + 16, 2);
}

// ============================================================
// Content
// ============================================================
void ProfileScreen::drawContent() {
    // Avatar circle with "F"
    tft.fillCircle(36, 56, 22, COLOR_PANEL);
    tft.drawCircle(36, 56, 22, COLOR_NEON);
    tft.setTextDatum(CC_DATUM);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.drawString("F", 36, 56, 2);

    // Name — font 1 for portrait to ensure it fits
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_NEON, COLOR_BG);
    tft.drawString(USER_NAME, 68, 34, 2);

    // Level + Title
    tft.setTextSize(1);
    tft.setTextColor(COLOR_NEON2, COLOR_BG);
    char buf[40];
    snprintf(buf, sizeof(buf), "Рівень %d  %s", USER_LEVEL, USER_TITLE);
    tft.drawString(buf, 68, 54, 1);
    tft.drawString("robocode.ua  Вступ до Програмування", 68, 66, 1);


    // Divider line
    tft.drawFastHLine(0, 86, DISP_WIDTH, COLOR_PANEL);
    tft.drawFastHLine(0, 87, DISP_WIDTH, COLOR_PANEL);

    // 2x2 Stat boxes — narrower for portrait
    drawStatBox(8,   94, 70, 40, "Досягнення", "21");
    drawStatBox(82,  94, 70, 40, "Технології", "12+");
    drawStatBox(156, 94, 70, 40, "Проєкти",    "8");
    drawStatBox(8,   142, 70, 40, "Kahoot Перемоги", ":)");


    // Badges label
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.drawString("ЗНАЧКИ:", 8, 192, 1);


    // Badge pills with green borders
    int bx = 8, by = 208;
    for (int i = 0; i < BADGE_COUNT; i++) {
        int bw = tft.textWidth(BADGES[i]) + 12; // measured in font 1
        // If doesn't fit on row, wrap
        if (bx + bw > DISP_WIDTH - 10) { bx = 8; by += 18; }
        tft.drawRoundRect(bx, by, bw, 15, 3, COLOR_NEON);
        tft.setTextColor(COLOR_NEON, COLOR_BG);
        tft.setTextDatum(CL_DATUM);
        tft.drawString(BADGES[i], bx + 6, by + 8, 1);
        bx += bw + 4;
    }

    // Back button at bottom
    int btnY = DISP_HEIGHT - 26;
    tft.drawRoundRect(50, btnY, 140, 18, 4, COLOR_DIM);
    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.setTextDatum(CC_DATUM);
    tft.setTextSize(1);
    tft.drawString("< НАЗАД", 120, btnY + 9, 1);

    tft.setTextDatum(TL_DATUM);
}

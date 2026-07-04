/**
 * GamesScreen.cpp — Mini Games Menu
 *
 * Lists available mini-games as touchable tiles.
 */

#include "GamesScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

const GamesScreen::GameTile GamesScreen::_games[4] = {
    { "SNAKE", "Snake" },
    { "REACT", "Reaction Test" },
    { "CALC",  "Calculator" },
    { "STOP",  "Stopwatch" },
};

GamesScreen::GamesScreen() {}

void GamesScreen::start() {
    tft.fillScreen(COLOR_BG);
    drawHeader("MINI GAMES", true);
    drawContent();
    drawStatusBar("ONLINE", "09:41");
}

bool GamesScreen::update() { return false; }

int GamesScreen::handleTap(int x, int y) {
    int tileW = (DISP_WIDTH - 30) / 2;
    int tileH = 55;
    int startY = HEADER_HEIGHT + 16;

    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int tx = 10 + col * (tileW + 10);
        int ty = startY + row * (tileH + 10);

        if (x >= tx && x <= tx + tileW && y >= ty && y <= ty + tileH) {
            // Highlight briefly
            highlightTile(tx, ty, tileW, tileH);
            return i;
        }
    }
    return -1;
}

void GamesScreen::drawContent() {
    int tileW = (DISP_WIDTH - 30) / 2;
    int tileH = 55;
    int startY = HEADER_HEIGHT + 16;

    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int tx = 10 + col * (tileW + 10);
        int ty = startY + row * (tileH + 10);

        drawCard(tx, ty, tileW, tileH, COLOR_DIM);

        // Icon
        tft.setTextColor(COLOR_NEON, COLOR_PANEL);
        tft.setTextSize(1);
        tft.setTextDatum(CL_DATUM);
        tft.drawString(_games[i].icon, tx + 8, ty + tileH / 2, 4);
        tft.setTextDatum(TL_DATUM);

        // Name
        tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
        tft.setTextSize(1);
        tft.drawString(_games[i].name, tx + 60, ty + tileH / 2 - 6, 2);
    }
}

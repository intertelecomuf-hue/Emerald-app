/**
 * AboutScreen.cpp — [MSG] From Family: typewriter-style letter from parents
 */

#include "AboutScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

AboutScreen::AboutScreen()
    : _currentLine(-1)
    , _lineStartTime(0)
    , _complete(false)
{
    _lines[0]  = "Fedir,";
    _lines[1]  = "";
    _lines[2]  = "You came a long way.";
    _lines[3]  = "From your first LED blink";
    _lines[4]  = "to your own console.";
    _lines[5]  = "From your first line of code";
    _lines[6]  = "to Space Shooter game.";
    _lines[7]  = "";
    _lines[8]  = "We are proud of you";
    _lines[9]  = "every single day.";
    _lines[10] = "";
    _lines[11] = "Keep building.";
    _lines[12] = "Keep dreaming.";
    _lines[13] = "This is just the beginning";
    _lines[14] = "of a great story.";
    _lines[15] = "";
    _lines[16] = "- Dad and Mom";
}

void AboutScreen::start() {
    _currentLine = -1;
    _lineStartTime = millis();
    _complete = false;

    tft.fillScreen(COLOR_BG);

    // Accent green vertical line on the left
    tft.fillRect(6, 8, 3, 300, COLOR_NEON);

    // Header "[ FROM FAMILY ]"
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_NEON2, COLOR_BG);
    tft.drawString("[ FROM FAMILY ]", 16, 8, 1);

    // Divider line below header
    tft.drawFastHLine(0, 22, DISP_WIDTH, COLOR_PANEL);

    // Back button at bottom
    int btnY = DISP_HEIGHT - 26;
    tft.drawRoundRect(50, btnY, 140, 18, 4, COLOR_DIM);
    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.setTextDatum(CC_DATUM);
    tft.setTextSize(1);
    tft.drawString("< BACK", 120, btnY + 9, 1);
    tft.setTextDatum(TL_DATUM);
}

bool AboutScreen::update() {
    if (_complete) return true;

    unsigned long now = millis();
    unsigned long elapsed = now - _lineStartTime;

    // Reveal a new line every 350ms
    int expectedLine = elapsed / 350;
    if (expectedLine > NUM_LINES - 1) expectedLine = NUM_LINES - 1;

    if (expectedLine > _currentLine) {
        _currentLine = expectedLine;
        drawLine(_currentLine);
    }

    // Auto-complete after all lines + extra delay
    if (_currentLine >= NUM_LINES - 1 && elapsed > 7000) {
        _complete = true;
    }

    return false;
}

void AboutScreen::handleTap(int x, int y) {
    (void)x; (void)y;
    // Tap to skip all animation and show full text
    _currentLine = NUM_LINES - 1;
    for (int i = 0; i < NUM_LINES; i++) {
        drawLine(i);
    }
    _complete = true;
}

void AboutScreen::drawLine(int index) {
    int startY = 28;     // below divider line
    int lineH = 13;      // spacing for 17 lines

    if (index < 0 || index >= NUM_LINES) return;

    int y = startY + index * lineH;
    const char* text = _lines[index];

    if (text[0] == '\0') return; // empty line — skip

    // Determine color
    uint16_t color = COLOR_TEXT;
    if (index == 0) color = COLOR_NEON;       // "Fedir," — bright neon
    if (index >= 8 && index <= 9) color = COLOR_NEON2; // "We are proud..."
    if (index == 16) color = COLOR_NEON2;     // "- Dad and Mom"

    tft.setTextColor(color, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(text, 24, y, 1);
    tft.setTextDatum(TL_DATUM);
}

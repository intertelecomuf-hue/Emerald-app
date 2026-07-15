/**
 * ConsoleScreen.cpp — Fake Terminal with touch keyboard
 *
 * Supported commands: help, whoami, date, status, scan, sensors, about, clear, fedir
 */

#include "ConsoleScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

// Keyboard layout
const char* ConsoleScreen::_keys[3] = {
    "1234567890<-",
    "QWERTYUIOP",
    "ASDFGHJKLENTER",
};
const int ConsoleScreen::_keyCols[3] = { 12, 10, 12 };

ConsoleScreen::ConsoleScreen()
    : _inputLen(0)
    , _scrollY(0)
    , _cursorBlink(0)
    , _historyCount(0)
{
    _input[0] = '\0';
    for (int i = 0; i < MAX_HISTORY; i++) {
        _history[i][0] = '\0';
    }
}

void ConsoleScreen::start() {
    tft.fillScreen(COLOR_BG);
    _scrollY = 0;
    _inputLen = 0;
    _input[0] = '\0';

    // Draw header
    drawHeader("СИСТЕМНА КОНСОЛЬ", true);

    drawKeyboard();

    // Welcome message
    clearScreen();
    printLine("Emerald Системна Консоль v1.0", COLOR_NEON2);
    printLine("Введи 'help' для списку команд.", COLOR_DIM);

    printLine("", COLOR_TEXT);
}

bool ConsoleScreen::update() {
    // Blink cursor
    _cursorBlink = millis() / 400;
    return false;
}

void ConsoleScreen::handleTap(int x, int y) {
    // Check if keyboard area
    int kbY = DISP_HEIGHT - 110;
    if (y < kbY) return;

    // Determine which key was pressed
    int row = -1;
    int col = -1;
    int keyW = 24;
    int keyH = 28;
    int gap = 2;

    for (int r = 0; r < 3; r++) {
        int totalW = _keyCols[r] * (keyW + gap) - gap;
        int startX = (DISP_WIDTH - totalW) / 2;
        int ry = kbY + r * (keyH + gap);

        if (y >= ry && y < ry + keyH) {
            row = r;
            int kx = x - startX;
            if (kx >= 0) {
                col = kx / (keyW + gap);
                if (col >= _keyCols[r]) col = _keyCols[r] - 1;
            }
            break;
        }
    }

    if (row < 0 || col < 0) return;

    char key = _keys[row][col];

    if (key == '<' && _keys[row][col + 1] == '-') {
        // Backspace
        if (_inputLen > 0) {
            _inputLen--;
            _input[_inputLen] = '\0';
        }
    } else if (key == 'E' && _keys[row][col + 1] == 'N' &&
               _keys[row][col + 2] == 'T' && _keys[row][col + 3] == 'E' &&
               _keys[row][col + 4] == 'R') {
        // Enter
        executeCommand(_input);
        _inputLen = 0;
        _input[0] = '\0';
    } else if (key >= 'A' && key <= 'Z') {
        // Letter or digit
        if (_inputLen < (int)sizeof(_input) - 2) {
            _input[_inputLen++] = tolower(key);
            _input[_inputLen] = '\0';
        }
    } else if (key >= '0' && key <= '9') {
        if (_inputLen < (int)sizeof(_input) - 2) {
            _input[_inputLen++] = key;
            _input[_inputLen] = '\0';
        }
    }

    // Redraw input area
    drawInputArea();
}

void ConsoleScreen::drawKeyboard() {
    int kbY = DISP_HEIGHT - 110;
    int keyW = 20;
    int keyH = 28;
    int gap = 2;

    // Keyboard background
    tft.fillRect(0, kbY - 4, DISP_WIDTH, 3 * (keyH + gap) + 8, COLOR_PANEL);
    tft.drawFastHLine(0, kbY - 4, DISP_WIDTH, COLOR_DIM);

    for (int r = 0; r < 3; r++) {
        int totalW = _keyCols[r] * (keyW + gap) - gap;
        int startX = (DISP_WIDTH - totalW) / 2;
        int ry = kbY + r * (keyH + gap);

        for (int c = 0; c < _keyCols[r]; c++) {
            char k = _keys[r][c];
            int kx = startX + c * (keyW + gap);

            // "ENTER" is a 4-char sequence, handle specially
            if (k == 'E' && _keys[r][c + 1] == 'N' &&
                _keys[r][c + 2] == 'T' && _keys[r][c + 3] == 'E' &&
                _keys[r][c + 4] == 'R') {
                // Draw ENTER key spanning 4 cols
                int enterW = 4 * (keyW + gap) - gap;
                tft.fillRoundRect(kx, ry, enterW, keyH, 3, COLOR_DIM);
                tft.drawRoundRect(kx, ry, enterW, keyH, 3, COLOR_NEON);
                tft.setTextColor(COLOR_NEON, COLOR_DIM);
                tft.setTextSize(1);
                tft.setTextDatum(CC_DATUM);
                tft.drawString("ENTER", kx + enterW / 2, ry + keyH / 2, 1);
                tft.setTextDatum(TL_DATUM);
                c += 4; // skip remaining chars of ENTER
                continue;
            }

            if (k == '<' && _keys[r][c + 1] == '-') {
                // Backspace key spanning 2 cols
                int bsW = 2 * (keyW + gap) - gap;
                tft.fillRoundRect(kx, ry, bsW, keyH, 3, COLOR_DIM);
                tft.drawRoundRect(kx, ry, bsW, keyH, 3, COLOR_DIM);
                tft.setTextColor(COLOR_TEXT, COLOR_DIM);
                tft.setTextSize(1);
                tft.setTextDatum(CC_DATUM);
                tft.drawString("DEL", kx + bsW / 2, ry + keyH / 2, 1);
                tft.setTextDatum(TL_DATUM);
                c += 1; // skip '-'
                continue;
            }

            // Regular key
            char buf[2] = { k, '\0' };
            tft.fillRoundRect(kx, ry, keyW, keyH, 3, COLOR_PANEL);
            tft.drawRoundRect(kx, ry, keyW, keyH, 3, COLOR_DIM);
            tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
            tft.setTextSize(1);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(buf, kx + keyW / 2, ry + keyH / 2, 1);
            tft.setTextDatum(TL_DATUM);
        }
    }
}

void ConsoleScreen::drawInputArea() {
    int inputY = DISP_HEIGHT - 125;
    tft.fillRect(8, inputY, DISP_WIDTH - 16, 16, COLOR_BG);

    // Prompt
    tft.setTextColor(COLOR_NEON2, COLOR_BG);
    tft.setTextSize(1);
    tft.drawString("$ ", 8, inputY, 2);

    // Input text
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(_input, 28, inputY, 2);

    // Cursor
    int cursorX = 28 + tft.textWidth(_input, 2);
    if (_cursorBlink % 2 == 0) {
        tft.fillRect(cursorX, inputY, 6, 12, COLOR_NEON);
    } else {
        tft.fillRect(cursorX, inputY, 6, 12, COLOR_BG);
    }
}

void ConsoleScreen::printLine(const char* line, uint16_t color) {
    int textStartY = HEADER_HEIGHT + 6;
    int lineH = 18;
    int maxLines = (DISP_HEIGHT - HEADER_HEIGHT - 135) / lineH;

    // Store in history
    if (_historyCount < MAX_HISTORY) {
        strncpy(_history[_historyCount], line, 63);
        _history[_historyCount][63] = '\0';
        _historyCount++;
    } else {
        // Shift history up
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(_history[i], _history[i + 1]);
        }
        strncpy(_history[MAX_HISTORY - 1], line, 63);
        _history[MAX_HISTORY - 1][63] = '\0';
        // Adjust scroll
        if (_scrollY > 0) _scrollY--;
    }

    // Redraw visible history
    tft.fillRect(2, textStartY, DISP_WIDTH - 4, maxLines * lineH, COLOR_BG);

    int startIdx = _historyCount - maxLines;
    if (startIdx < 0) startIdx = 0;

    for (int i = startIdx; i < _historyCount; i++) {
        int y = textStartY + (i - startIdx) * lineH;
        tft.setTextColor(COLOR_TEXT, COLOR_BG);
        tft.setTextSize(1);
        tft.drawString(_history[i], 8, y, 1);
    }
}

void ConsoleScreen::clearScreen() {
    _historyCount = 0;
    _scrollY = 0;
    int textStartY = HEADER_HEIGHT + 6;
    static const int LINE_H = 18;
    int maxLines = (DISP_HEIGHT - HEADER_HEIGHT - 135) / LINE_H;
    tft.fillRect(2, textStartY, DISP_WIDTH - 4, maxLines * LINE_H, COLOR_BG);
}

void ConsoleScreen::executeCommand(const char* cmd) {
    char displayCmd[72];
    snprintf(displayCmd, sizeof(displayCmd), "$ %s", cmd);
    printLine(displayCmd, COLOR_NEON2);

    if (strcmp(cmd, "help") == 0) {
        printLine("Komandy:", COLOR_NEON);
        printLine("  help     - Pokazaty cej dovidnyk", COLOR_TEXT);
        printLine("  whoami   - Informaciya pro korystuvacha", COLOR_TEXT);
        printLine("  date     - Data ta chas", COLOR_TEXT);
        printLine("  status   - Stan systemy", COLOR_TEXT);
        printLine("  scan     - Skannuvaty WiFi", COLOR_TEXT);
        printLine("  sensors  - Dani z sensoriv", COLOR_TEXT);
        printLine("  about    - Pro Emerald", COLOR_TEXT);
        printLine("  clear    - Ochystyty ekran", COLOR_TEXT);
        printLine("  fedir    - Tayemne povidomlennya", COLOR_TEXT);
    } else if (strcmp(cmd, "whoami") == 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s@emerald x %s x Level %d",
                 USER_NAME, USER_TITLE, USER_LEVEL);
        printLine(buf, COLOR_NEON);
    } else if (strcmp(cmd, "date") == 0) {
        printLine("Systemnyy chas: 09:41", COLOR_TEXT);
        printLine("Systemna data: 2026-05-28", COLOR_TEXT);
    } else if (strcmp(cmd, "status") == 0) {
        printLine("Systema: ONLINE", COLOR_NEON);
        printLine("CPU: 240MHz", COLOR_TEXT);
        printLine("Dosyahnennya: 21/21", COLOR_TEXT);
        printLine("Prystroyiv pidklyucheno: 0", COLOR_TEXT);
    } else if (strcmp(cmd, "scan") == 0) {
        printLine("Zapusk WiFi skanuvannya...", COLOR_DIM);
        printLine("Dlya detaley vykorystay ekran Wi-Fi.", COLOR_TEXT);
    } else if (strcmp(cmd, "sensors") == 0) {
        printLine("Temp CPU: 42.5 C", COLOR_TEXT);
        printLine("Napruga systemy: 3.3V", COLOR_TEXT);
        printLine("Syhnal WiFi: -54 dBm", COLOR_TEXT);
    } else if (strcmp(cmd, "about") == 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s %s", OS_NAME, OS_VERSION);
        printLine(buf, COLOR_NEON);
        printLine("Personalnyy hadzhet dlya yunyh prohramistiv.", COLOR_TEXT);
        printLine("Stvoreno dlya Fedira.", COLOR_TEXT);
    } else if (strcmp(cmd, "clear") == 0) {
        clearScreen();
    } else if (strcmp(cmd, "fedir") == 0) {
        printLine("Maybutnye nalezhy`t` tobi, Fedir.", COLOR_NEON);
    } else if (strcmp(cmd, "") == 0) {
        // Empty command, do nothing
    } else {
        char buf[64];
        snprintf(buf, sizeof(buf), "Nevidoma komanda: %s", cmd);
        printLine(buf, COLOR_RED);
    }
}

/**
 * ConsoleScreen.h — Fake Terminal with touch keyboard
 */

#ifndef CONSOLE_SCREEN_H
#define CONSOLE_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class ConsoleScreen {
public:
    ConsoleScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

private:
    char _input[64];
    int _inputLen;
    int _scrollY;
    unsigned long _cursorBlink;

    static const int MAX_HISTORY = 12;
    char _history[MAX_HISTORY][64];
    int _historyCount;

    void drawKeyboard();
    void drawInputArea();
    void executeCommand(const char* cmd);
    void printLine(const char* line, uint16_t color);
    void clearScreen();

    // Keyboard layout
    static const char* _keys[3];
    static const int _keyCols[3];
};

#endif // CONSOLE_SCREEN_H

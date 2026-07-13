/**
 * VirtualKeyboard.h — On-screen keyboard for WiFi password entry
 *
 * Row layout:
 *   Row 0: Q W E R T Y U I O P
 *   Row 1: A S D F G H J K L
 *   Row 2: Z X C V B N M
 *   Row 3: 1 2 3 4 5 6 7 8 9 0
 *   Row 4: . @ - _ !   [SPACE]  [BS] [CLR] [OK]
 *
 * Tap on [OK] returns true to signal "connect now".
 */

#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#define KEYBOARD_MAX_PASS 32

class VirtualKeyboard {
public:
    VirtualKeyboard();

    /** Call once to show the keyboard for a given SSID */
    void begin(const char* ssid);

    /** Call every loop(); returns false while active, true when [OK] pressed */
    bool update();

    /** Handle touch; returns true if [OK] was pressed (call getPassword after) */
    bool handleTap(int x, int y);

    /** Get the entered password */
    const char* getPassword() const;

    /** Get the SSID this keyboard was opened for */
    const char* getSSID() const;

private:
    struct KeyDef {
        char label[4];
        char value;       // 0 = special action
        int  x, y, w, h;
    };

    static const int KEY_ROWS = 5;
    static const int KEYS_PER_ROW = 10;
    static const int MAX_KEYS = KEY_ROWS * KEYS_PER_ROW;

    KeyDef _keys[MAX_KEYS];
    int    _keyCount;

    char   _ssid[33];
    char   _password[KEYBOARD_MAX_PASS];
    int    _passLen;
    bool   _done;

    // Layout
    int _kbX, _kbY, _kbW, _kbH;
    int _inputY;

    void buildLayout();
    void draw();
    void drawInputField();
    void drawKeys();
    void highlightKey(int idx);
    void addChar(char c);
    void backspace();
    void clearPassword();
};

#endif // VIRTUAL_KEYBOARD_H

/**
 * MenuScreen.h — Simple screen with single Wi-Fi tile
 */

#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#define MENU_TILE_SPY       0
#define MENU_TILE_COUNT     1

class MenuScreen {
public:
    MenuScreen();

    void start();
    bool update();

    /** Returns 0 if Wi-Fi tile pressed, -1 if none */
    int handleTap(int x, int y);

    int selectedTile();
    void highlightTile(int index);

private:
    struct MenuTile {
        const char* icon;
        const char* title;
    };

    static const MenuTile _tiles[MENU_TILE_COUNT];

    // Single large tile centered
    static const int TILE_W = 200;
    static const int TILE_H = 120;

    int _selectedTile;
    unsigned long _highlightEnd;

    void drawTile(int index, bool highlight);
    void drawHeader();
    void drawStatusBarInternal();
    int hitTest(int x, int y);
};

#endif // MENU_SCREEN_H

/**
 * MenuScreen.h — Main Menu (2×4 grid)
 *
 * Tiles: Developer Profile, Mission Archive, Wi-Fi,
 *        Mini Games, Toolbox, System Console, About Device
 */

#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// Tiles
#define MENU_TILE_PROFILE   0
#define MENU_TILE_ARCHIVE   1
#define MENU_TILE_SPY       2
#define MENU_TILE_SETTINGS  3
#define MENU_TILE_GAMES     4
#define MENU_TILE_CONSOLE   5
#define MENU_TILE_ABOUT     6

#define MENU_TILE_COUNT     7

class MenuScreen {
public:
    MenuScreen();

    void start();
    bool update();

    /** Returns which tile was pressed, or -1 if none */
    int handleTap(int x, int y);

    /** Which tile is currently selected (for transition) */
    int selectedTile();
    void highlightTile(int index);

private:
    struct MenuTile {
        const char* icon;
        const char* title;
        const char* subtitle;
    };

    static const MenuTile _tiles[MENU_TILE_COUNT];

    // Tile layout: 2 columns, 4 rows (last row has 1 tile centered)
    // No subtitle — icon + title only, vertically centered
    static const int TILE_W = 110;
    static const int TILE_H = 60;      // taller for portrait
    static const int TILE_GAP = 4;     // tighter gap
    static const int GRID_X = 6;
    static const int GRID_Y = 32;      // shifted up slightly

    int _selectedTile;
    unsigned long _highlightEnd;

    void drawTile(int index, bool highlight);
    void drawHeader();
    void drawStatusBarInternal();
    int hitTest(int x, int y);
};

#endif // MENU_SCREEN_H

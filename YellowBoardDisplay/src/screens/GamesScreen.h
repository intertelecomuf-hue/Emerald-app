/**
 * GamesScreen.h — Mini Games Menu
 */

#ifndef GAMES_SCREEN_H
#define GAMES_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class GamesScreen {
public:
    GamesScreen();

    void start();
    bool update();
    int handleTap(int x, int y);

private:
    struct GameTile {
        const char* icon;
        const char* name;
    };

    static const GameTile _games[4];

    void drawContent();
};

#endif // GAMES_SCREEN_H

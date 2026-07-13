/**
 * ArchiveScreen.h — [MAP] My Missions: numbered list, alternating rows
 */

#ifndef ARCHIVE_SCREEN_H
#define ARCHIVE_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

struct Mission {
    const char* icon;
    const char* name;
};

class ArchiveScreen {
public:
    ArchiveScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

private:
    void drawContent();
};

#endif // ARCHIVE_SCREEN_H

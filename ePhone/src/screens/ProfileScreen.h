/**
 * ProfileScreen.h — [USR] My Profile: avatar, stats, badges
 */

#ifndef PROFILE_SCREEN_H
#define PROFILE_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class ProfileScreen {
public:
    ProfileScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

private:
    void drawContent();
    void drawStatBox(int x, int y, int w, int h,
                     const char* label, const char* value);
};

#endif // PROFILE_SCREEN_H

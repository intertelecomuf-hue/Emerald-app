/**
 * AboutScreen.h — Parent message with typewriter effect
 */

#ifndef ABOUT_SCREEN_H
#define ABOUT_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class AboutScreen {
public:
    AboutScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

private:
    static const int NUM_LINES = 17;
    const char* _lines[NUM_LINES];

    int _currentLine;
    unsigned long _lineStartTime;
    bool _complete;

    void drawLine(int index);
};

#endif // ABOUT_SCREEN_H

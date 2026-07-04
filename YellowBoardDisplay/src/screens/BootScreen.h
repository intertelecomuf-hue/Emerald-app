/**
 * BootScreen.h — Emerald Boot Sequence
 *
 * Phases (non-blocking, driven by millis()):
 *   0: Black screen (300ms)
 *   1: Logo + glow + version
 *   2: Terminal lines appear one by one
 *   3: Progress bar fills alongside lines
 *   4: Fade out → transition to STORY
 */

#ifndef BOOT_SCREEN_H
#define BOOT_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class BootScreen {
public:
    BootScreen();

    /** Call once when entering BOOT state */
    void start();

    /** Call every frame during BOOT state. Returns true when complete. */
    bool update();

    /** True when boot is done and ready to transition */
    bool isComplete();

private:
    int _phase;
    unsigned long _phaseStart;
    unsigned long _lastLineTime;
    int _currentLine;
    int _progressPercent;
    bool _complete;

    // Boot lines text
    static const int NUM_LINES = 6;
    static const char* _lines[NUM_LINES];

    void drawPhase1();     // Black screen
    void drawPhase2();     // Logo + version
    void drawPhase3();     // Terminal lines
    void drawPhase4();     // Progress bar update
    void drawPhase5();     // Fade out

    /** Draw the Emerald crystal image — a faceted green diamond */
    static void drawEmeraldImage();
};

#endif // BOOT_SCREEN_H

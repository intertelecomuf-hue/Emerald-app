/**
 * StoryScreen.h — Achievement Story Mode
 *
 * Displays achievement cards one by one with animations.
 * Tap to advance to next card. Last card → Final Slide → MENU.
 */

#ifndef STORY_SCREEN_H
#define STORY_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

struct Achievement {
    const char* icon;
    const char* title;
    const char* line1;
    const char* line1Bold;  // highlighted portion of line1
    const char* line2;
    const char* line2Bold;  // highlighted portion of line2
};

class StoryScreen {
public:
    StoryScreen();

    void start();
    bool update();    // returns true when story is complete → MENU

    // Touch handling
    void handleTap(int x, int y);

private:
    int _currentCard;
    bool _complete;

    // Animation state
    unsigned long _cardStartTime;
    bool _showTapHint;

    // Timing (ms)
    static const unsigned long CARD_MIN_DISPLAY = 1500;  // min 1.5s per card
    static const unsigned long SLIDE_IN_DURATION = 500;  // phase delay before tap hint
    static const unsigned long FINAL_SLIDE_DELAY = 3000; // 3s then auto → MENU

    // Internal phases per card
    enum CardPhase {
        CARD_SLIDE_IN,
        CARD_SHOWING,
        CARD_FINAL    // Final Slide shown, waiting for tap/timeout
    };
    CardPhase _cardPhase;
    unsigned long _phaseStart;

    void drawAchievementCard(int index);
    void drawFinalSlide();
    void drawDots();
};

#endif // STORY_SCREEN_H

/**
 * StoryScreen.cpp — Achievement Story Mode
 *
 * Shows achievement cards from the array, one per tap.
 * Last card → Final Slide → MENU.
 */

#include "StoryScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"
#include "../ui/Animations.h"

extern TFT_eSPI tft;

// ============================================================
// Achievement Data (English — TFT_eSPI has no Cyrillic support)
// ============================================================
static const int NUM_ACHIEVEMENTS = 10;

static const Achievement achievements[NUM_ACHIEVEMENTS] = {
    { "[DICE]",  "Random Inventor",
      "I wrote a",             "number generator",
      "and launched my ",      "digital dice." },

    { "[LED]",   "LED Commander",
      "I programmed",          "RGB LEDs",
      "and made them ",        "shine my way." },

    { "[GEAR]",  "Motor Mechanic",
      "I connected",           "servo motors",
      "- my first step ",      "into robotics." },

    { "[WIFI]",  "IoT Pioneer",
      "I sent data",           "to the internet",
      "via ",                  "ThingSpeak." },

    { "[PHONE]", "App Inventor",
      "I built my own",        "mobile app",
      "and launched it ",      "on a phone." },

    { "[GAME]",  "Space Shooter Dev",
      "I developed",           "Space Shooter",
      "with accelerometer ",   "control." },

    { "[BT]",    "Bluetooth Engineer",
      "I set up",              "HC-05",
      "and built wireless ",   "control." },

    { "[CTRL]",  "Console Engineer",
      "I soldered",            "my own console",
      "on Raspberry ",         "Pi Pico." },

    { "[WEB]",   "Web Developer",
      "I built a",             "web app",
      "and ran it on ",        "iPhone 4s." },

    { "[CUP]",   "Kahoot Champion",
      "I won at",              "Kahoot",
      "my laptop is full ",    "of stickers!" },
};

// ============================================================
StoryScreen::StoryScreen()
    : _currentCard(0)
    , _complete(false)
    , _cardStartTime(0)
    , _showTapHint(false)
    , _cardPhase(CARD_SLIDE_IN)
    , _phaseStart(0)
{
}

void StoryScreen::start() {
    _currentCard = 0;
    _complete = false;
    _cardPhase = CARD_SLIDE_IN;
    _phaseStart = millis();
    _cardStartTime = millis();

    tft.fillScreen(COLOR_BLACK);
    drawAchievementCard(0);
}

bool StoryScreen::update() {
    if (_complete) return true;

    unsigned long now = millis();
    unsigned long elapsed = now - _phaseStart;

    // ============================================================
    // Phase transition: CARD_SLIDE_IN → CARD_SHOWING (after 500ms)
    // During SLIDE_IN, tap is ignored to prevent accidental advance
    // ============================================================
    if (_cardPhase == CARD_SLIDE_IN && elapsed >= SLIDE_IN_DURATION) {
        _cardPhase = CARD_SHOWING;
    }

    // ============================================================
    // CARD_FINAL: Final Slide — wait for tap or timeout (3s → MENU)
    // ============================================================
    if (_cardPhase == CARD_FINAL) {
        // Auto-advance to MENU after FINAL_SLIDE_DELAY
        if (elapsed >= FINAL_SLIDE_DELAY) {
            _complete = true;
            return true;
        }
        return false;
    }

    // ============================================================
    // CARD_SHOWING: Tap hint blinking + dots
    // ============================================================
    if (_cardPhase == CARD_SHOWING) {
        // Blink tap hint every 800ms
        bool hintVisible = (now / 800) % 2 == 0;
        if (hintVisible != _showTapHint) {
            _showTapHint = hintVisible;
            int hintY = DISP_HEIGHT - 50;
            tft.fillRect(0, hintY - 4, DISP_WIDTH, 16, COLOR_BLACK);

            if (_showTapHint) {
                tft.setTextColor(COLOR_DIM, COLOR_BLACK);
                tft.setTextSize(1);
                tft.setTextDatum(CC_DATUM);
                tft.drawString("[ tap to continue ]", DISP_WIDTH / 2, hintY, 2);
                tft.setTextDatum(TL_DATUM);
            }

            // Redraw dots when hint redraws
            drawDots();
        }
    }

    return false;
}

void StoryScreen::handleTap(int x, int y) {
    (void)x; (void)y;
    if (_complete) return;

    unsigned long cardAge = millis() - _phaseStart;

    // ============================================================
    // CARD_FINAL: Tap on Final Slide → complete → MENU
    // ============================================================
    if (_cardPhase == CARD_FINAL) {
        _complete = true;
        return;
    }

    // ============================================================
    // During SLIDE_IN or early SHOWING: ignore tap
    // Wait at least CARD_MIN_DISPLAY (1.5s) to let user read
    // ============================================================
    if (cardAge < CARD_MIN_DISPLAY) return;

    if (_currentCard < NUM_ACHIEVEMENTS - 1) {
        // Next card
        _currentCard++;
        _cardPhase = CARD_SLIDE_IN;
        _phaseStart = millis();
        _cardStartTime = millis();
        _showTapHint = false;

        // Clear and draw
        tft.fillScreen(COLOR_BLACK);
        drawAchievementCard(_currentCard);

    } else {
        // Last card → Show Final Slide (NOT _complete yet!)
        _cardPhase = CARD_FINAL;
        _phaseStart = millis();
        _showTapHint = false;

        tft.fillScreen(COLOR_BLACK);
        drawFinalSlide();
    }
}

void StoryScreen::drawAchievementCard(int index) {
    const Achievement &a = achievements[index];
    int cx = DISP_WIDTH / 2;
    int cardY = 20;     // shifted up
    int cardW = DISP_WIDTH - 20;
    int cardH = DISP_HEIGHT - 55;  // taller card: ends at y=205 (was y=200)

    // Card background — темно-зелений (COLOR_PANEL) + неонова рамка
    drawCard(10, cardY, cardW, cardH, COLOR_NEON);

    // "ACHIEVEMENT UNLOCKED" label
    tft.setTextColor(COLOR_NEON2, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("ACHIEVEMENT UNLOCKED", cx, cardY + 18, 2);
    tft.setTextDatum(TL_DATUM);

    // Icon
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(2);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(a.icon, cx, cardY + 55, 4);
    tft.setTextDatum(TL_DATUM);

    // Title
    tft.setTextColor(COLOR_NEON, COLOR_PANEL);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(a.title, cx, cardY + 90, 4);
    tft.setTextDatum(TL_DATUM);

    // Description lines with bold highlights
    int textY = cardY + 125;
    int textX = 30;
    int lineH = 22;

    // Line 1: normal + bold
    tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
    tft.setTextSize(1);
    tft.drawString(a.line1, textX, textY, 2);

    int w1 = tft.textWidth(a.line1, 2);
    tft.setTextColor(COLOR_HIGHLIGHT, COLOR_PANEL);
    tft.drawString(a.line1Bold, textX + w1, textY, 2);

    // Line 2: normal + bold
    textY += lineH;
    tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
    tft.drawString(a.line2, textX, textY, 2);

    int w2 = tft.textWidth(a.line2, 2);
    tft.setTextColor(COLOR_HIGHLIGHT, COLOR_PANEL);
    tft.drawString(a.line2Bold, textX + w2, textY, 2);

    // Dots
    drawDots();
}

void StoryScreen::drawFinalSlide() {
    tft.fillScreen(COLOR_BG);

    // "Fedir." — manual glow with pixel-offset copies
    tft.setTextDatum(CC_DATUM);
    tft.setTextSize(3);

    // Two faded glow copies at offset positions
    tft.setTextColor(TFT_COLOR(20, 80, 40), COLOR_BG);
    tft.drawString("Fedir.", (DISP_WIDTH / 2) + 1, (DISP_HEIGHT / 2) - 29);
    tft.drawString("Fedir.", (DISP_WIDTH / 2) - 1, (DISP_HEIGHT / 2) - 31);

    // Main bright text
    tft.setTextColor(COLOR_NEON, COLOR_BG);
    tft.drawString("Fedir.", DISP_WIDTH / 2, DISP_HEIGHT / 2 - 30);

    // Subtitle
    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString("This is only the beginning.", DISP_WIDTH / 2, DISP_HEIGHT / 2 + 15);

    // Welcome button with frame
    int btnW = 240;
    int btnH = 30;
    int btnX = (DISP_WIDTH - btnW) / 2;
    int btnY = DISP_HEIGHT / 2 + 45;
    tft.drawRoundRect(btnX, btnY, btnW, btnH, 6, COLOR_NEON);
    tft.setTextColor(COLOR_NEON, COLOR_BG);
    tft.drawString("> Welcome to Emerald", DISP_WIDTH / 2, btnY + btnH / 2);

    // Tap hint
    tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.drawString("[ tap to continue ]", DISP_WIDTH / 2, DISP_HEIGHT - 40);

    tft.setTextDatum(TL_DATUM);
}

void StoryScreen::drawDots() {
    int dotY = DISP_HEIGHT - 30;
    int spacing = 14;
    int total = NUM_ACHIEVEMENTS;
    int startX = (DISP_WIDTH - (total * spacing)) / 2 + spacing / 2;

    // Clear dot area
    tft.fillRect(0, dotY - 5, DISP_WIDTH, 12, COLOR_BLACK);

    for (int i = 0; i < total; i++) {
        int x = startX + i * spacing;
        uint16_t color = (i <= _currentCard) ? COLOR_NEON : COLOR_DIM;
        tft.fillCircle(x, dotY, 3, color);
    }
}

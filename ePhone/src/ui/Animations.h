/**
 * Animations.h — Visual effects: fade, slide, typewriter, glow, scanlines
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// Fade In/Out (via PWM on backlight or overlay)
// ============================================================
void fadeIn(int durationMs);
void fadeOut(int durationMs);

// ============================================================
// Slide transition (new content slides up from bottom)
// ============================================================
void slideInFromBottom(int durationMs);

// ============================================================
// Scanline overlay effect
// ============================================================
void drawScanlines();

// ============================================================
// Glow text (draw text multiple times with offset + dimmer colors)
// ============================================================
void drawGlowText(const char* text, int x, int y,
                  uint8_t font, uint16_t color);

// ============================================================
// Blinking cursor / tap indicator
// ============================================================
void drawBlinker(int x, int y, bool visible);

// ============================================================
// Typewriter effect line (non-blocking, called each frame)
// Returns true when animation is complete
// ============================================================
struct TypewriterAnim {
    const char* text;
    int x, y;
    uint16_t color;
    unsigned long startTime;
    int charDelayMs;
    int totalChars;
    bool done;
};

void startTypewriter(TypewriterAnim &anim, const char* text,
                     int x, int y, uint16_t color, int charDelayMs);
bool updateTypewriter(TypewriterAnim &anim);

// ============================================================
// Boot line animation (fade-in from left)
// ============================================================
struct BootLine {
    const char* text;
    int x, y;
    uint16_t color;
    unsigned long appearTime;
    bool visible;
};

void animateBootLine(BootLine &line, unsigned long now);

#endif // ANIMATIONS_H

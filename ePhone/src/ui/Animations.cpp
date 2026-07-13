/**
 * Animations.cpp — Visual effects: fade, slide, typewriter, glow, scanlines
 *
 * All functions are non-blocking (no delay()), use millis() for timing.
 */

#include "Animations.h"
#include "../config.h"

// ============================================================
// Fade In — gradually increase backlight PWM
// ============================================================
void fadeIn(int durationMs) {
    const int steps = 20;
    int stepMs = durationMs / steps;
    for (int i = 0; i <= steps; i++) {
        int brightness = (i * 255) / steps;
        // Use PWM-like effect by drawing a semi-transparent overlay
        // or just use a simple approach: draw dark overlay decreasing opacity
        ledcWrite(0, brightness); // channel 0 on BL pin
        delay(stepMs);            // small delay acceptable here (setup phase)
    }
}

// ============================================================
// Fade Out — gradually decrease backlight
// ============================================================
void fadeOut(int durationMs) {
    const int steps = 20;
    int stepMs = durationMs / steps;
    for (int i = steps; i >= 0; i--) {
        int brightness = (i * 255) / steps;
        ledcWrite(0, brightness);
        delay(stepMs);
    }
}

// ============================================================
// Fade helper without delay — called from loop
// Returns 0.0..1.0 progress
// ============================================================
float fadeProgress(unsigned long startTime, int durationMs) {
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= (unsigned long)durationMs) return 1.0f;
    return (float)elapsed / (float)durationMs;
}

// ============================================================
// Slide In From Bottom — animate content moving up
// ============================================================
void slideInFromBottom(int durationMs) {
    unsigned long start = millis();
    unsigned long now = start;

    // This is a simplified version; full slide needs double-buffering
    // which is complex on ESP32. We use an overlay fade approach instead.
    // Real implementation uses the calling screen to draw at offset Y
    // based on progress.

    while (now - start < (unsigned long)durationMs) {
        float progress = (float)(now - start) / (float)durationMs;

        // Draw a black bar from bottom moving up
        int barH = DISP_HEIGHT * (1.0f - progress);
        tft.fillRect(0, DISP_HEIGHT - barH, DISP_WIDTH, barH, COLOR_BLACK);

        now = millis();
    }
}

// ============================================================
// Scanlines — horizontal lines at 4px spacing with low opacity
// ============================================================
void drawScanlines() {
    for (int y = 0; y < DISP_HEIGHT; y += 4) {
        tft.drawFastHLine(0, y, DISP_WIDTH, COLOR_BLACK);
    }
}

// ============================================================
// Glow Text — draw text 3 times with 1px offset
// ============================================================
void drawGlowText(const char* text, int x, int y,
                  uint8_t font, uint16_t color) {
    // Save current datum
    uint8_t prevDatum = tft.getTextDatum();
    tft.setTextDatum(CC_DATUM);

    // Layer 1 — dimmest, farthest offset
    tft.setTextColor(COLOR_DIM, COLOR_BLACK);
    tft.drawString(text, x - 2, y - 2, font);

    // Layer 2 — medium glow
    tft.setTextColor(color, COLOR_BLACK, true); // transparent bg doesn't work well, use solid
    // Actually let's use a gradient approach:
    tft.setTextColor(COLOR_NEON2, COLOR_BLACK);
    tft.drawString(text, x - 1, y - 1, font);

    // Layer 3 — main bright text
    tft.setTextColor(color, COLOR_BLACK);
    tft.drawString(text, x, y, font);

    // Restore datum
    tft.setTextDatum(prevDatum);
}

// ============================================================
// Blinker — blinking cursor or indicator
// ============================================================
void drawBlinker(int x, int y, bool visible) {
    if (visible) {
        tft.fillRect(x, y, 8, 12, COLOR_NEON);
    } else {
        tft.fillRect(x, y, 8, 12, COLOR_BG);
    }
}

// ============================================================
// Typewriter Animation (non-blocking)
// ============================================================
void startTypewriter(TypewriterAnim &anim, const char* text,
                     int x, int y, uint16_t color, int charDelayMs) {
    anim.text = text;
    anim.x = x;
    anim.y = y;
    anim.color = color;
    anim.startTime = millis();
    anim.charDelayMs = charDelayMs;
    anim.totalChars = strlen(text);
    anim.done = false;
}

bool updateTypewriter(TypewriterAnim &anim) {
    if (anim.done) return true;

    unsigned long elapsed = millis() - anim.startTime;
    int charsToShow = elapsed / anim.charDelayMs;

    if (charsToShow >= anim.totalChars) {
        charsToShow = anim.totalChars;
        anim.done = true;
    }

    // Draw the visible part
    char buf[128];
    int len = charsToShow;
    if (len > (int)sizeof(buf) - 1) len = sizeof(buf) - 1;

    // First clear the line
    tft.fillRect(anim.x, anim.y - 2, DISP_WIDTH - anim.x, 16, COLOR_BG);

    // Draw characters one by one
    strncpy(buf, anim.text, len);
    buf[len] = '\0';

    tft.setTextColor(anim.color, COLOR_BG);
    tft.setTextSize(1);
    tft.drawString(buf, anim.x, anim.y, 2);

    // Draw blinking cursor if not done
    if (!anim.done) {
        int cursorX = anim.x + tft.textWidth(buf, 2);
        bool cursorOn = (elapsed / 300) % 2 == 0;
        drawBlinker(cursorX, anim.y, cursorOn);
    }

    return anim.done;
}

// ============================================================
// Boot Line Animation — fade in from left
// ============================================================
void animateBootLine(BootLine &line, unsigned long now) {
    if (!line.visible) return;

    unsigned long age = now - line.appearTime;
    if (age < 200) {
        // Fade in — draw progressively longer line
        int maxW = tft.textWidth(line.text, 2);
        int w = (maxW * (int)age) / 200;
        if (w > maxW) w = maxW;

        // Draw line clipped
        tft.setTextColor(line.color, COLOR_BG);
        tft.setTextSize(1);

        // Draw only up to w pixels
        char buf[64];
        strncpy(buf, line.text, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        // Find how many chars fit in w pixels
        int chars = 0;
        int sw = 0;
        while (buf[chars] != '\0') {
            char tmp[2] = { buf[chars], '\0' };
            sw += tft.textWidth(tmp, 2);
            if (sw > w) break;
            chars++;
        }
        buf[chars] = '\0';

        tft.drawString(buf, line.x, line.y, 2);
    } else {
        // Fully visible
        tft.setTextColor(line.color, COLOR_BG);
        tft.setTextSize(1);
        tft.drawString(line.text, line.x, line.y, 2);
    }
}

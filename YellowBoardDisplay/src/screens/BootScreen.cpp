/**
 * BootScreen.cpp — Emerald Boot Sequence
 *
 * Non-blocking boot animation using millis() state machine.
 */

#include "BootScreen.h"
#include "../config.h"
#include "../ui/Animations.h"
#include "../ui/UIComponents.h"

extern TFT_eSPI tft;

// Boot terminal lines — transliterated (TFT_eSPI has no Cyrillic support)
const char* BootScreen::_lines[NUM_LINES] = {
    "> Zapusk yadra Emerald...",
    "> Zavantazhennya drayveriv...",
    "> Montazh faylovoyi systemy...",
    "> Kalibrovka sensoriv...",
    "> Pidklyuchennya do maybutnogo...",
    "OK  Emerald gotovyy."
};

BootScreen::BootScreen()
    : _phase(0)
    , _phaseStart(0)
    , _lastLineTime(0)
    , _currentLine(-1)
    , _progressPercent(0)
    , _complete(false)
{
}

void BootScreen::start() {
    _phase = 0;
    _phaseStart = millis();
    _lastLineTime = 0;
    _currentLine = -1;
    _progressPercent = 0;
    _complete = false;

    tft.fillScreen(COLOR_BG);
}

bool BootScreen::isComplete() {
    return _complete;
}

bool BootScreen::update() {
    if (_complete) return true;

    unsigned long now = millis();
    unsigned long elapsed = now - _phaseStart;

    switch (_phase) {
        // ============================================================
        // Phase 0: Black screen (300ms)
        // ============================================================
        case 0: {
            if (elapsed >= 300) {
                _phase = 1;
                _phaseStart = now;
            }
            break;
        }

        // ============================================================
        // Phase 1: Logo "Emerald" with glow + version
        // ============================================================
        case 1: {
            tft.fillScreen(COLOR_BG);

            // Glow text: Emerald — на темно-зеленому фоні
            drawGlowText(OS_NAME, DISP_WIDTH / 2, DISP_HEIGHT / 2 - 20, 6, COLOR_NEON);

            // Version below
            char verBuf[48];
            snprintf(verBuf, sizeof(verBuf), "%s x %s", OS_VERSION, OS_EDITION);
            tft.setTextColor(COLOR_DIM, COLOR_BG);
            tft.setTextSize(1);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(verBuf, DISP_WIDTH / 2, DISP_HEIGHT / 2 + 20, 2);
            tft.setTextDatum(TL_DATUM);

            // Small delay to show logo
            if (elapsed >= 1500) {
                _phase = 2;
                _phaseStart = now;
                _lastLineTime = now;
                _currentLine = -1;
                // Clear for terminal lines — true black for terminal look
                tft.fillScreen(COLOR_BLACK);
            }
            break;
        }

        // ============================================================
        // Phase 2: Terminal lines appearing one by one
        // ============================================================
        case 2: {
            // Check if it's time for the next line
            if (_currentLine < NUM_LINES - 1 &&
                now - _lastLineTime >= BOOT_LINE_DELAY) {

                _currentLine++;
                _lastLineTime = now;

                // Draw the new line — compact spacing for portrait mode
                int lineY = 8 + _currentLine * 16;
                uint16_t lineColor = (_currentLine == NUM_LINES - 1)
                                     ? COLOR_NEON2   // last line is green checkmark
                                     : COLOR_TEXT;

                const char* prefix = (_currentLine == NUM_LINES - 1) ? "" : "";
                char displayText[64];
                snprintf(displayText, sizeof(displayText), "%s%s",
                         prefix, _lines[_currentLine]);

                tft.setTextColor(lineColor, COLOR_BLACK);
                tft.setTextSize(1);
                tft.drawString(displayText, 10, lineY, 2);
            }

            // If all lines shown, wait a moment then go to progress phase
            if (_currentLine >= NUM_LINES - 1 && now - _lastLineTime >= 800) {
                _phase = 3;
                _phaseStart = now;
                _progressPercent = 0;
            }
            break;
        }

        // ============================================================
        // Phase 3: Progress bar (fills alongside the already-shown lines)
        // ============================================================
        case 3: {
            // Calculate progress based on time (phase duration ~1.5s)
            unsigned long progElapsed = now - _phaseStart;
            _progressPercent = (progElapsed * 100) / 1500;
            if (_progressPercent > 100) _progressPercent = 100;

            // Draw progress bar at bottom
            int barY = DISP_HEIGHT - 50;
            drawProgressBar(20, barY, DISP_WIDTH - 80, 16,
                           _progressPercent, COLOR_NEON);

            if (_progressPercent >= 100) {
                // Show Emerald image (drawn once)
                if (now - _phaseStart >= 1800) {
                    _phase = 4;
                    _phaseStart = now;
                    drawEmeraldImage();
                }
            }
            break;
        }

        // ============================================================
        // Phase 4: Emerald image splash (1.5s)
        // ============================================================
        case 4: {
            // Just wait — the emerald image stays on screen
            if (now - _phaseStart >= 1800) {
                _phase = 5;
                _phaseStart = now;
            }
            break;
        }

        // ============================================================
        // Phase 5: Fade out (draw black overlay with increasing opacity)
        // ============================================================
        case 5: {
            unsigned long fadeElapsed = now - _phaseStart;
            int alpha = (fadeElapsed * 255) / FADE_DURATION_MS;
            if (alpha > 255) alpha = 255;

            // Simple fade: draw progressively darker overlay
            int lines = (alpha * DISP_HEIGHT) / 255 / 4;
            for (int i = 0; i < lines; i++) {
                tft.drawFastHLine(0, i * 4, DISP_WIDTH, COLOR_BLACK);
            }

            if (fadeElapsed >= (unsigned long)FADE_DURATION_MS + 200) {
                _complete = true;
                tft.fillScreen(COLOR_BLACK);
            }
            break;
        }
    }

    return _complete;
}

// ============================================================
// Emerald crystal image — drawn as a faceted gem shape
// A green diamond/crystal representing the Emerald OS logo
// ============================================================
void BootScreen::drawEmeraldImage() {
    int cx = DISP_WIDTH / 2;   // 160
    int cy = DISP_HEIGHT / 2;  // 120

    // Clear screen to very dark green
    tft.fillScreen(TFT_COLOR(4, 8, 5));

    // Dark outer glow circles
    for (int r = 65; r >= 50; r--) {
        uint8_t brightness = map(r, 50, 65, 0, 60);
        tft.drawCircle(cx, cy, r, TFT_COLOR(0, brightness, brightness / 3));
    }

    // ==========================================
    // Draw faceted emerald crystal (diamond shape)
    // ==========================================

    // Main crystal body — elongated octagon / diamond
    // Points (top to bottom, clockwise):
    //   top:    (cx,   cy-50)
    //   t-r:    (cx+30, cy-25)
    //   r:      (cx+40, cy)
    //   b-r:    (cx+30, cy+25)
    //   bottom: (cx,   cy+50)
    //   b-l:    (cx-30, cy+25)
    //   l:      (cx-40, cy)
    //   t-l:    (cx-30, cy-25)

    // Dark facets (left side — shadow)
    tft.fillTriangle(cx, cy-50, cx-30, cy-25, cx-40, cy,       TFT_COLOR(20, 90, 45));
    tft.fillTriangle(cx, cy-50, cx-40, cy,   cx-30, cy+25,     TFT_COLOR(15, 70, 35));
    tft.fillTriangle(cx-30, cy+25, cx-40, cy, cx, cy+50,       TFT_COLOR(10, 50, 25));

    // Bright facets (right side — light)
    tft.fillTriangle(cx, cy-50, cx+30, cy-25, cx+40, cy,       TFT_COLOR(50, 200, 100));
    tft.fillTriangle(cx, cy-50, cx+40, cy,   cx+30, cy+25,     TFT_COLOR(60, 220, 110));
    tft.fillTriangle(cx+30, cy+25, cx+40, cy, cx, cy+50,       TFT_COLOR(40, 160, 80));

    // Center facet (highlight)
    tft.fillTriangle(cx, cy, cx-20, cy-15, cx+20, cy-15,       TFT_COLOR(80, 240, 130));
    tft.fillTriangle(cx, cy, cx-20, cy+15, cx+20, cy+15,       TFT_COLOR(70, 220, 115));

    // Bright sparkle in top-right
    tft.fillTriangle(cx+5, cy-40, cx+20, cy-30, cx+10, cy-25,  TFT_COLOR(150, 255, 180));

    // Border lines
    tft.drawLine(cx, cy-50, cx+30, cy-25, TFT_COLOR(100, 255, 150));
    tft.drawLine(cx, cy-50, cx-30, cy-25, TFT_COLOR(80, 200, 100));
    tft.drawLine(cx+30, cy-25, cx+40, cy, TFT_COLOR(100, 255, 150));
    tft.drawLine(cx-30, cy-25, cx-40, cy, TFT_COLOR(80, 200, 100));
    tft.drawLine(cx+40, cy, cx+30, cy+25, TFT_COLOR(100, 255, 150));
    tft.drawLine(cx-40, cy, cx-30, cy+25, TFT_COLOR(80, 200, 100));
    tft.drawLine(cx+30, cy+25, cx, cy+50, TFT_COLOR(100, 255, 150));
    tft.drawLine(cx-30, cy+25, cx, cy+50, TFT_COLOR(80, 200, 100));

    // Center horizontal cut line
    tft.drawLine(cx-38, cy, cx+38, cy, TFT_COLOR(40, 160, 80));

    // Vertical cut line (top half)
    tft.drawLine(cx, cy-48, cx, cy, TFT_COLOR(40, 160, 80));

    // ==========================================
    // "EMERALD" text below the crystal
    // ==========================================
    tft.setTextColor(COLOR_NEON, TFT_COLOR(4, 8, 5));
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("EMERALD", cx, cy + 68, 4);
    tft.setTextDatum(TL_DATUM);
}

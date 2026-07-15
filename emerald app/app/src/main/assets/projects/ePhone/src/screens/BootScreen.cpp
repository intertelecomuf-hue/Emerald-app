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
    , _emeraldX(-100)
    , _emeraldAnimStart(0)
    , _emeraldAnimPhase(0)
    , _emeraldAngle(0)
    , _emeraldWordDrawn(false)
    , _emeraldLetterDrawn(false)
    , _emeraldCoreDrawn(false)
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

void BootScreen::handleTap() {
    // Skip boot animation on tap — jump to complete
    _complete = true;
    tft.fillScreen(COLOR_BLACK);
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
                // Start animated Emerald sequence
                if (now - _phaseStart >= 1800) {
                    _phase = 4;
                    _phaseStart = now;
                    // Init emerald animation state
                    _emeraldX = -100;
                    _emeraldAnimPhase = 0;
                    _emeraldAnimStart = now;
                    _emeraldAngle = 0;
                    _emeraldCoreDrawn = false;
                    _emeraldLetterDrawn = false;
                    _emeraldWordDrawn = false;
                    // Clear screen to very dark green
                    tft.fillScreen(TFT_COLOR(4, 8, 5));
                }
            }
            break;
        }

        // ============================================================
        // Phase 4: Animated Emerald crystal (fly-in, spin, core, letter, word)
        // ============================================================
        case 4: {
            drawEmeraldAnimated();
            if (_emeraldAnimPhase >= 5) {
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
// Animated Emerald crystal — inspired by the HTML animation
// Phases: 0=fly-in, 1=spin, 2=core appear, 3=letter appear, 4=word appear, 5=done
// ============================================================
void BootScreen::drawEmeraldAnimated() {
    unsigned long now = millis();
    unsigned long elapsed = now - _emeraldAnimStart;

    switch (_emeraldAnimPhase) {
        // ============================================================
        // Phase 0: Fly-in from left (0–600ms)
        // ============================================================
        case 0: {
            drawEmeraldFlyIn();
            if (elapsed >= 600) {
                _emeraldAnimPhase = 1;
                _emeraldAnimStart = now;
                _emeraldAngle = 45;
            }
            break;
        }

        // ============================================================
        // Phase 1: Spin the crystal (600–2600ms, 360° over 2s)
        // ============================================================
        case 1: {
            unsigned long spinElapsed = now - _emeraldAnimStart;
            _emeraldAngle = 45 + (spinElapsed * 360.0f) / 2000.0f;
            if (_emeraldAngle >= 405) _emeraldAngle = 405;
            drawEmeraldFlyIn();  // redraw at center with current angle
            if (spinElapsed >= 2000) {
                _emeraldAnimPhase = 2;
                _emeraldAnimStart = now;
                _emeraldAngle = 45;
            }
            break;
        }

        // ============================================================
        // Phase 2: Core appears (2600–3200ms, 600ms delay then core)
        // ============================================================
        case 2: {
            // Redraw crystal at final position
            drawEmeraldFlyIn();
            if (elapsed >= 600 && !_emeraldCoreDrawn) {
                drawEmeraldCore();
                _emeraldCoreDrawn = true;
            }
            if (elapsed >= 1200) {
                _emeraldAnimPhase = 3;
                _emeraldAnimStart = now;
            }
            break;
        }

        // ============================================================
        // Phase 3: Letter appears (3200–3600ms)
        // ============================================================
        case 3: {
            drawEmeraldFlyIn();
            if (!_emeraldCoreDrawn) {
                drawEmeraldCore();
                _emeraldCoreDrawn = true;
            }
            if (elapsed >= 400 && !_emeraldLetterDrawn) {
                drawEmeraldLetter();
                _emeraldLetterDrawn = true;
            }
            if (elapsed >= 1200) {
                _emeraldAnimPhase = 4;
                _emeraldAnimStart = now;
            }
            break;
        }

        // ============================================================
        // Phase 4: Word "EMERALD" appears below (3600–4600ms)
        // ============================================================
        case 4: {
            drawEmeraldFlyIn();
            if (!_emeraldCoreDrawn) {
                drawEmeraldCore();
                _emeraldCoreDrawn = true;
            }
            if (!_emeraldLetterDrawn) {
                drawEmeraldLetter();
                _emeraldLetterDrawn = true;
            }
            if (elapsed >= 800 && !_emeraldWordDrawn) {
                drawEmeraldWord();
                _emeraldWordDrawn = true;
            }
            if (elapsed >= 2000) {
                _emeraldAnimPhase = 5;
            }
            break;
        }
    }
}

// ============================================================
// Draw the Emerald crystal at current _emeraldX and _emeraldAngle
// The crystal is a rotated square (diamond) with faceted shading
// ============================================================
void BootScreen::drawEmeraldFlyIn() {
    int cx = DISP_WIDTH / 2;   // 160
    int cy = DISP_HEIGHT / 2;  // 120

    // Calculate fly-in X position (ease-out cubic)
    if (_emeraldAnimPhase == 0) {
        unsigned long elapsed = millis() - _emeraldAnimStart;
        float t = (float)elapsed / 600.0f;
        if (t > 1.0f) t = 1.0f;
        // Cubic ease-out: 1 - (1-t)^3
        float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
        _emeraldX = (int)(-100 + (cx + 100) * ease);
    } else {
        _emeraldX = cx;
    }

    // Clear previous frame with a dark circle (covers rotated square corners)
    tft.fillCircle(_emeraldX, cy, 58, TFT_COLOR(4, 8, 5));

    // Crystal size
    float s = 45.0f; // half-size of the square (before rotation)

    // Pre-compute rotated points of a square centered at (_emeraldX, cy)
    float angleRad = _emeraldAngle * PI / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    // 4 corners of a square: top, right, bottom, left (before rotation)
    int corners[4][2];
    float pts[4][2] = {{0.0f, -s}, {s, 0.0f}, {0.0f, s}, {-s, 0.0f}};
    for (int i = 0; i < 4; i++) {
        float x = pts[i][0] * cosA - pts[i][1] * sinA;
        float y = pts[i][0] * sinA + pts[i][1] * cosA;
        corners[i][0] = (int)(_emeraldX + x);
        corners[i][1] = (int)(cy + y);
    }

    // Draw faceted crystal using triangles
    // Dark facets (left side — shadow)
    tft.fillTriangle(corners[0][0], corners[0][1],
                     corners[3][0], corners[3][1],
                     corners[2][0], corners[2][1],
                     TFT_COLOR(15, 70, 35));

    // Bright facets (right side — light)
    tft.fillTriangle(corners[0][0], corners[0][1],
                     corners[1][0], corners[1][1],
                     corners[2][0], corners[2][1],
                     TFT_COLOR(50, 200, 100));

    // Center highlight facet
    int cx2 = (_emeraldX + corners[0][0] + corners[1][0] + corners[2][0] + corners[3][0]) / 4;
    int cy2 = (cy + corners[0][1] + corners[1][1] + corners[2][1] + corners[3][1]) / 4;
    tft.fillTriangle(corners[0][0], corners[0][1],
                     cx2, cy2,
                     corners[1][0], corners[1][1],
                     TFT_COLOR(80, 240, 130));

    // Border lines
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        tft.drawLine(corners[i][0], corners[i][1],
                     corners[j][0], corners[j][1],
                     TFT_COLOR(100, 255, 150));
    }

    // Glow circles behind crystal
    for (int r = 60; r >= 50; r--) {
        uint8_t brightness = map(r, 50, 60, 0, 40);
        tft.drawCircle(_emeraldX, cy, r, TFT_COLOR(0, brightness, brightness / 3));
    }
}

// ============================================================
// Draw white core inside the crystal
// ============================================================
void BootScreen::drawEmeraldCore() {
    int cx = DISP_WIDTH / 2;
    int cy = DISP_HEIGHT / 2;

    // White core — smaller rotated square inside
    float s = 22.0f;
    float angleRad = _emeraldAngle * PI / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    int corners[4][2];
    float pts[4][2] = {{0.0f, -s}, {s, 0.0f}, {0.0f, s}, {-s, 0.0f}};
    for (int i = 0; i < 4; i++) {
        float x = pts[i][0] * cosA - pts[i][1] * sinA;
        float y = pts[i][0] * sinA + pts[i][1] * cosA;
        corners[i][0] = (int)(cx + x);
        corners[i][1] = (int)(cy + y);
    }

    // Fill core as two triangles
    tft.fillTriangle(corners[0][0], corners[0][1],
                     corners[1][0], corners[1][1],
                     corners[2][0], corners[2][1],
                     TFT_COLOR(220, 255, 240));
    tft.fillTriangle(corners[0][0], corners[0][1],
                     corners[2][0], corners[2][1],
                     corners[3][0], corners[3][1],
                     TFT_COLOR(200, 245, 230));

    // Core border
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        tft.drawLine(corners[i][0], corners[i][1],
                     corners[j][0], corners[j][1],
                     TFT_COLOR(255, 255, 255));
    }
}

// ============================================================
// Draw letter "E" in the center
// ============================================================
void BootScreen::drawEmeraldLetter() {
    int cx = DISP_WIDTH / 2;
    int cy = DISP_HEIGHT / 2;

    tft.setTextColor(TFT_COLOR(10, 100, 50), TFT_COLOR(4, 8, 5));
    tft.setTextSize(2);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("E", cx, cy, 4);
    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Draw "EMERALD" word below the crystal
// ============================================================
void BootScreen::drawEmeraldWord() {
    int cx = DISP_WIDTH / 2;
    int cy = DISP_HEIGHT / 2;

    tft.setTextColor(COLOR_NEON, TFT_COLOR(4, 8, 5));
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("EMERALD", cx, cy + 68, 4);
    tft.setTextDatum(TL_DATUM);
}

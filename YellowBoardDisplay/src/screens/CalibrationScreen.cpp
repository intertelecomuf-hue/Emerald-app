/**
 * CalibrationScreen.cpp — Touch Calibration with 3×3 grid
 *
 * Grid layout (portrait 240×320):
 *   +----+----+----+
 *   | 1  | 2  | 3  |
 *   +----+----+----+
 *   | 4  | 5  | 6  |
 *   +----+----+----+
 *   | 7  | 8  | 9  |
 *   +----+----+----+
 *
 * For each point, 3 raw ADC samples are taken and averaged.
 * After all 9 points, calibration is computed and a test screen shown.
 */

#include "CalibrationScreen.h"
#include "../config.h"
#include "../ui/UIComponents.h"
#include "../services/TouchManager.h"

extern TFT_eSPI tft;
extern TouchManager touch;

// Cell dimensions
static const int CELL_W = (DISP_WIDTH - 20) / 3;   // ~73
static const int CELL_H = (DISP_HEIGHT - 60) / 3;  // ~86
static const int GRID_X = 10;
static const int GRID_Y = 30;

CalibrationScreen::CalibrationScreen()
    : _currentPoint(-1)
    , _done(false)
    , _testMode(false)
    , _waitingRelease(false)
    , _collectingSamples(false)
    , _sampleStartTime(0)
    , _calRawMinX(0), _calRawMaxX(0)
    , _calRawMinY(0), _calRawMaxY(0)
    , _calSwapXY(false), _calInvertX(false), _calInvertY(false)
{
    // Pre-calculate screen center coordinates for each cell
    for (int i = 0; i < NUM_POINTS; i++) {
        int col = i % GRID_COLS;
        int row = i / GRID_COLS;
        _screenX[i] = GRID_X + col * CELL_W + CELL_W / 2;
        _screenY[i] = GRID_Y + row * CELL_H + CELL_H / 2;
        _rawX[i] = -1;
        _rawY[i] = -1;
        _sampleSumX[i] = 0;
        _sampleSumY[i] = 0;
        _sampleCount[i] = 0;
    }
}

void CalibrationScreen::start() {
    _currentPoint = -1;
    _done = false;
    _testMode = false;
    _waitingRelease = false;
    _collectingSamples = false;
    _sampleStartTime = 0;

    // Reset raw data
    for (int i = 0; i < NUM_POINTS; i++) {
        _rawX[i] = -1;
        _rawY[i] = -1;
        _sampleSumX[i] = 0;
        _sampleSumY[i] = 0;
        _sampleCount[i] = 0;
    }

    tft.fillScreen(COLOR_BLACK);

    // Title
    tft.setTextColor(COLOR_NEON, COLOR_BLACK);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("TOUCH CALIBRATION", DISP_WIDTH / 2, 10, 2);
    tft.setTextDatum(TL_DATUM);

    // Draw empty grid
    drawGrid();

    // Status bar
    drawStatusBarType("Tap cell 1 to start", COLOR_YELLOW);

    Serial.println();
    Serial.println("========================================");
    Serial.println("  TOUCH CALIBRATION STARTED");
    Serial.println("  Grid: 3×3, 9 points");
    Serial.println("  Hold finger on each highlighted cell");
    Serial.println("  until 3 samples are collected");
    Serial.println("========================================");
    Serial.println();
}

bool CalibrationScreen::update() {
    if (_done) return true;
    
    // If collecting samples, try to read raw touch
    if (_collectingSamples && _currentPoint >= 0 && _currentPoint < NUM_POINTS) {
        unsigned long now = millis();
        
        // Timeout: if we've been collecting too long, move on
        if (now - _sampleStartTime > SAMPLE_WINDOW_MS) {
            Serial.printf("[Calib] Point %d: timeout after %dms, got %d samples\n",
                          _currentPoint + 1, SAMPLE_WINDOW_MS, _sampleCount[_currentPoint]);
            _collectingSamples = false;
            _waitingRelease = true;
            finalizePoint();
            return false;
        }
        
        // Try to read raw touch
        uint16_t rx = 0, ry = 0;
        if (touch.getTouchRaw(&rx, &ry, 200)) {
            _sampleSumX[_currentPoint] += rx;
            _sampleSumY[_currentPoint] += ry;
            _sampleCount[_currentPoint]++;
            
            Serial.printf("[Calib] Point %d: sample %d/%d  raw=(%d,%d)\n",
                          _currentPoint + 1, _sampleCount[_currentPoint], NUM_SAMPLES, rx, ry);
            
            if (_sampleCount[_currentPoint] >= NUM_SAMPLES) {
                // Got enough samples, average them
                _collectingSamples = false;
                _waitingRelease = true;
                finalizePoint();
            }
        }
    }
    
    return false;
}

void CalibrationScreen::finalizePoint() {
    // Average the samples
    int idx = _currentPoint;
    _rawX[idx] = _sampleSumX[idx] / _sampleCount[idx];
    _rawY[idx] = _sampleSumY[idx] / _sampleCount[idx];
    
    int pointId = idx + 1;
    Serial.printf("[Calib] Point %d FINAL: screen=(%d,%d)  raw_avg=(%d,%d)  samples=%d\n",
                  pointId,
                  _screenX[idx], _screenY[idx],
                  _rawX[idx], _rawY[idx],
                  _sampleCount[idx]);
    
    // Mark captured on screen
    drawPoint(idx, false);  // remove highlight
    // Draw a small green dot where the cell center is
    tft.fillCircle(_screenX[idx], _screenY[idx], 4, COLOR_NEON);
    
    _currentPoint++;
    
    if (_currentPoint >= NUM_POINTS) {
        // All 9 points captured
        computeCalibration();
        printSummary();
        _testMode = true;
        drawTestScreen();
        return;
    }
    
    // Highlight next point
    drawPoint(_currentPoint, true);
    
    // Update status
    char status[32];
    snprintf(status, sizeof(status), "Hold cell %d", _currentPoint + 1);
    drawStatusBarType(status, COLOR_NEON);
}

void CalibrationScreen::handleTap(int x, int y) {
    if (_done) return;
    
    // In test mode, handle test screen taps
    if (_testMode) {
        // Map raw touch to screen using current calibration
        // and draw a dot where the system thinks the touch is
        uint16_t rx = 0, ry = 0;
        if (touch.getTouchRaw(&rx, &ry, 200)) {
            // Apply our calibration mapping
            int mx = rx, my = ry;
            if (_calSwapXY) {
                int t = mx; mx = my; my = t;
            }
            // Scale
            int rangeX = _calRawMaxX - _calRawMinX;
            int rangeY = _calRawMaxY - _calRawMinY;
            if (rangeX < 1) rangeX = 1;
            if (rangeY < 1) rangeY = 1;
            
            int sx = (mx - _calRawMinX) * DISP_WIDTH / rangeX;
            int sy = (my - _calRawMinY) * DISP_HEIGHT / rangeY;
            
            if (_calInvertX) sx = DISP_WIDTH - sx;
            if (_calInvertY) sy = DISP_HEIGHT - sy;
            
            // Clamp
            if (sx < 0) sx = 0; if (sx >= DISP_WIDTH) sx = DISP_WIDTH - 1;
            if (sy < 0) sy = 0; if (sy >= DISP_HEIGHT) sy = DISP_HEIGHT - 1;
            
            // Draw dot
            tft.fillCircle(sx, sy, 3, TFT_COLOR(255, 100, 100));
            
            Serial.printf("[Test] raw=(%d,%d)  mapped=(%d,%d)\n", rx, ry, sx, sy);
        }
        return;
    }
    
    // Ignore taps in status bar area
    if (y >= DISP_HEIGHT - STATUSBAR_HEIGHT) return;
    
    // If we're waiting for release, check if finger is lifted
    if (_waitingRelease) {
        // Check if touch is released
        uint16_t rx = 0, ry = 0;
        if (!touch.getTouchRaw(&rx, &ry, 200)) {
            _waitingRelease = false;
            // Start collecting samples for next point
            if (_currentPoint >= 0 && _currentPoint < NUM_POINTS) {
                _collectingSamples = true;
                _sampleStartTime = millis();
                _sampleSumX[_currentPoint] = 0;
                _sampleSumY[_currentPoint] = 0;
                _sampleCount[_currentPoint] = 0;
                
                char status[32];
                snprintf(status, sizeof(status), "Hold cell %d", _currentPoint + 1);
                drawStatusBarType(status, COLOR_NEON);
            }
        }
        return;
    }
    
    // First tap — start calibration
    if (_currentPoint == -1) {
        _currentPoint = 0;
        drawPoint(0, true);  // highlight first cell
        _collectingSamples = true;
        _sampleStartTime = millis();
        _sampleSumX[0] = 0;
        _sampleSumY[0] = 0;
        _sampleCount[0] = 0;
        drawStatusBarType("Hold cell 1", COLOR_NEON);
        Serial.println("[Calib] Starting — hold finger on cell 1");
        return;
    }
}

void CalibrationScreen::drawGrid() {
    // Draw cell borders
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            int cx = GRID_X + col * CELL_W;
            int cy = GRID_Y + row * CELL_H;
            tft.drawRect(cx, cy, CELL_W, CELL_H, COLOR_DIM);
        }
    }

    // Draw point numbers in each cell
    for (int i = 0; i < NUM_POINTS; i++) {
        drawPoint(i, false);
    }
}

void CalibrationScreen::drawPoint(int index, bool highlight) {
    int col = index % GRID_COLS;
    int row = index / GRID_COLS;
    int cx = GRID_X + col * CELL_W;
    int cy = GRID_Y + row * CELL_H;

    uint16_t bg = highlight ? COLOR_NEON : COLOR_BG;
    uint16_t fg = highlight ? COLOR_BLACK : COLOR_DIM;

    // Fill cell background if highlighted
    if (highlight) {
        tft.fillRect(cx + 1, cy + 1, CELL_W - 2, CELL_H - 2, bg);
    } else {
        tft.fillRect(cx + 1, cy + 1, CELL_W - 2, CELL_H - 2, COLOR_BG);
    }

    // Redraw cell border
    tft.drawRect(cx, cy, CELL_W, CELL_H, highlight ? COLOR_NEON : COLOR_DIM);

    // Point number
    char num[4];
    snprintf(num, sizeof(num), "%d", index + 1);
    tft.setTextColor(fg, bg);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(num, cx + CELL_W / 2, cy + CELL_H / 2, 4);
    tft.setTextDatum(TL_DATUM);
}

void CalibrationScreen::computeCalibration() {
    // Find min/max raw values
    _calRawMinX = _rawX[0];
    _calRawMaxX = _rawX[0];
    _calRawMinY = _rawY[0];
    _calRawMaxY = _rawY[0];
    
    for (int i = 1; i < NUM_POINTS; i++) {
        if (_rawX[i] < _calRawMinX) _calRawMinX = _rawX[i];
        if (_rawX[i] > _calRawMaxX) _calRawMaxX = _rawX[i];
        if (_rawY[i] < _calRawMinY) _calRawMinY = _rawY[i];
        if (_rawY[i] > _calRawMaxY) _calRawMaxY = _rawY[i];
    }
    
    // Determine if X/Y are swapped
    // Compare horizontal vs vertical spread in raw space
    int rawSpreadX = _calRawMaxX - _calRawMinX;
    int rawSpreadY = _calRawMaxY - _calRawMinY;
    
    // Screen is 240 wide, 320 tall
    // If raw X spread > raw Y spread, axes are likely swapped
    _calSwapXY = (rawSpreadX > rawSpreadY);
    
    // Determine inversion
    // Point 0 is top-left (screen_x=small, screen_y=small)
    // Point 2 is top-right (screen_x=large, screen_y=small)
    // Point 6 is bottom-left (screen_x=small, screen_y=large)
    // Point 8 is bottom-right (screen_x=large, screen_y=large)
    
    int rawTL, rawTR, rawBL, rawBR;
    if (_calSwapXY) {
        rawTL = _rawY[0];  // swapped: rawY is horizontal
        rawTR = _rawY[2];
        rawBL = _rawY[6];
        rawBR = _rawY[8];
    } else {
        rawTL = _rawX[0];
        rawTR = _rawX[2];
        rawBL = _rawX[6];
        rawBR = _rawX[8];
    }
    
    // If raw value at right (point 2) < raw value at left (point 0), X is inverted
    _calInvertX = (rawTR < rawTL);
    
    if (_calSwapXY) {
        rawTL = _rawX[0];  // swapped: rawX is vertical
        rawTR = _rawX[2];
        rawBL = _rawX[6];
        rawBR = _rawX[8];
    } else {
        rawTL = _rawY[0];
        rawTR = _rawY[2];
        rawBL = _rawY[6];
        rawBR = _rawY[8];
    }
    
    // If raw value at bottom (point 6) < raw value at top (point 0), Y is inverted
    _calInvertY = (rawBL < rawTL);
    
    // Adjust min/max based on inversion
    if (_calInvertX) {
        int t = _calRawMinX;
        _calRawMinX = _calRawMaxX;
        _calRawMaxX = t;
    }
    if (_calInvertY) {
        int t = _calRawMinY;
        _calRawMinY = _calRawMaxY;
        _calRawMaxY = t;
    }
}

void CalibrationScreen::printSummary() {
    Serial.println();
    Serial.println("============================================================");
    Serial.println("  CALIBRATION RESULTS — 9 POINTS (averaged)");
    Serial.println("============================================================");
    Serial.println("  point_id | screen_x | screen_y | raw_x_avg | raw_y_avg | samples");
    Serial.println("  ---------+----------+----------+-----------+-----------+--------");
    
    for (int i = 0; i < NUM_POINTS; i++) {
        Serial.printf("  %5d    |  %5d   |  %5d   |   %5d   |   %5d   |   %d\n",
                      i + 1,
                      _screenX[i], _screenY[i],
                      _rawX[i], _rawY[i],
                      _sampleCount[i]);
    }
    
    Serial.println("============================================================");
    Serial.println();
    Serial.println("  ANALYSIS:");
    Serial.printf("  Raw X range: %d .. %d (spread=%d)\n", _calRawMinX, _calRawMaxX, _calRawMaxX - _calRawMinX);
    Serial.printf("  Raw Y range: %d .. %d (spread=%d)\n", _calRawMinY, _calRawMaxY, _calRawMaxY - _calRawMinY);
    Serial.printf("  Swap XY:     %s\n", _calSwapXY ? "YES" : "NO");
    Serial.printf("  Invert X:    %s\n", _calInvertX ? "YES" : "NO");
    Serial.printf("  Invert Y:    %s\n", _calInvertY ? "YES" : "NO");
    Serial.println();
    Serial.println("  Suggested config.h values:");
    Serial.printf("  #define TOUCH_X_MIN    %d\n", _calRawMinX);
    Serial.printf("  #define TOUCH_X_MAX    %d\n", _calRawMaxX);
    Serial.printf("  #define TOUCH_Y_MIN    %d\n", _calRawMinY);
    Serial.printf("  #define TOUCH_Y_MAX    %d\n", _calRawMaxY);
    Serial.printf("  #define TOUCH_SWAP_XY  %s\n", _calSwapXY ? "true" : "false");
    Serial.printf("  #define TOUCH_INVERT_X %s\n", _calInvertX ? "true" : "false");
    Serial.printf("  #define TOUCH_INVERT_Y %s\n", _calInvertY ? "true" : "false");
    Serial.println();
    Serial.println("  Now in TEST MODE: tap anywhere on screen.");
    Serial.println("  Red dots show where the system maps your touch.");
    Serial.println("  Green dots show expected calibration cell centers.");
    Serial.println("============================================================");
    Serial.println();
}

void CalibrationScreen::drawTestScreen() {
    tft.fillScreen(COLOR_BLACK);
    
    tft.setTextColor(COLOR_NEON, COLOR_BLACK);
    tft.setTextSize(1);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("CALIBRATION TEST", DISP_WIDTH / 2, 15, 2);
    
    tft.setTextColor(COLOR_TEXT, COLOR_BLACK);
    tft.drawString("Tap anywhere to test mapping", DISP_WIDTH / 2, 40, 1);
    tft.drawString("Red dot = mapped position", DISP_WIDTH / 2, 55, 1);
    
    // Draw expected calibration points as green dots
    for (int i = 0; i < NUM_POINTS; i++) {
        tft.fillCircle(_screenX[i], _screenY[i] + 40, 3, COLOR_NEON);
    }
    
    // Draw grid lines (faint)
    for (int row = 0; row <= GRID_ROWS; row++) {
        int y = GRID_Y + row * CELL_H;
        tft.drawFastHLine(GRID_X, y, GRID_COLS * CELL_W, TFT_COLOR(20, 40, 25));
    }
    for (int col = 0; col <= GRID_COLS; col++) {
        int x = GRID_X + col * CELL_W;
        tft.drawFastVLine(x, GRID_Y, GRID_ROWS * CELL_H, TFT_COLOR(20, 40, 25));
    }
    
    tft.setTextColor(COLOR_YELLOW, COLOR_BLACK);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("Long press to finish & go to menu", DISP_WIDTH / 2, DISP_HEIGHT - 15, 1);
    tft.setTextDatum(TL_DATUM);
    
    drawStatusBarType("TEST MODE", COLOR_YELLOW);
}

bool CalibrationScreen::isDone() const {
    return _done;
}

bool CalibrationScreen::isTestMode() const {
    return _testMode;
}

void CalibrationScreen::reset() {
    _done = false;
    _testMode = false;
    _currentPoint = -1;
    _waitingRelease = false;
    _collectingSamples = false;
}

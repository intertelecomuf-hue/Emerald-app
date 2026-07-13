/**
 * CalibrationScreen.h — Touch Calibration with 3×3 grid
 *
 * Shows a 3×3 grid on screen. User taps each highlighted cell in sequence.
 * For each point, 3 raw touch samples are taken and averaged.
 * After all 9 points, a summary table is printed to Serial.
 * Then a test screen allows verifying the calibration.
 *
 * This screen runs FIRST after boot, before the main menu.
 */

#ifndef CALIBRATION_SCREEN_H
#define CALIBRATION_SCREEN_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class CalibrationScreen {
public:
    CalibrationScreen();

    void start();
    bool update();
    void handleTap(int x, int y);

    bool isDone() const;
    bool isTestMode() const;
    void reset();

    // Calibration results (populated after all 9 points)
    int getRawMinX() const { return _calRawMinX; }
    int getRawMaxX() const { return _calRawMaxX; }
    int getRawMinY() const { return _calRawMinY; }
    int getRawMaxY() const { return _calRawMaxY; }
    bool getSwapXY() const { return _calSwapXY; }
    bool getInvertX() const { return _calInvertX; }
    bool getInvertY() const { return _calInvertY; }

private:
    // Grid configuration: 3×3
    static const int GRID_COLS = 3;
    static const int GRID_ROWS = 3;
    static const int NUM_POINTS = 9;
    static const int NUM_SAMPLES = 3;  // samples per point
    static const int SAMPLE_WINDOW_MS = 3000;  // max time to collect samples

    // Screen coordinates for each calibration point (center of cell)
    int _screenX[NUM_POINTS];
    int _screenY[NUM_POINTS];

    // Raw touch coordinates captured for each point (averaged)
    int _rawX[NUM_POINTS];
    int _rawY[NUM_POINTS];

    // Per-point sample accumulation
    int _sampleSumX[NUM_POINTS];
    int _sampleSumY[NUM_POINTS];
    int _sampleCount[NUM_POINTS];
    unsigned long _sampleStartTime;

    int _currentPoint;       // 0..8, -1 = waiting to start
    bool _done;
    bool _testMode;          // after calibration, show test screen
    bool _waitingRelease;    // wait for finger to lift before next point
    bool _collectingSamples; // currently collecting samples for current point

    // Calibration results
    int _calRawMinX, _calRawMaxX;
    int _calRawMinY, _calRawMaxY;
    bool _calSwapXY, _calInvertX, _calInvertY;

    void drawGrid();
    void drawPoint(int index, bool highlight);
    void printSummary();
    void drawDoneScreen();
    void drawTestScreen();
    void computeCalibration();
    void finalizePoint();
};

#endif // CALIBRATION_SCREEN_H

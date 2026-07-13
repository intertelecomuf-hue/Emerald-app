#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

enum TouchActionType : uint8_t {
    TOUCH_NONE       = 0,
    TOUCH_TAP        = 1,
    TOUCH_LONG_PRESS = 2,
};

struct TouchEvent {
    bool pressed;
    int x, y;
    TouchActionType action;
};

class TouchManager {
public:
    TouchManager();
    void begin(TFT_eSPI* tft);
    TouchEvent getTouch();
    
    // Read raw ADC values directly from touch controller (no calibration)
    bool getTouchRaw(uint16_t *rawX, uint16_t *rawY, uint16_t threshold = 600);

private:
    TFT_eSPI* _tft;
    bool _wasTouching;
    unsigned long _pressTime;
    int _pressX, _pressY;
    bool _longReported;
    static const unsigned long LONG_PRESS_MS = 600;
};

#endif

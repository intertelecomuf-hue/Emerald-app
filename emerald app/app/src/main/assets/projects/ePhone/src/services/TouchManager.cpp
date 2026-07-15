#include "TouchManager.h"

TouchManager::TouchManager()
    : _tft(nullptr)
    , _wasTouching(false)
    , _pressTime(0)
    , _pressX(0), _pressY(0)
    , _longReported(false)
{}

void TouchManager::begin(TFT_eSPI* tft) {
    _tft = tft;
    Serial.println("[Touch] XPT2046 ready");
}

TouchEvent TouchManager::getTouch() {
    TouchEvent ev;
    ev.pressed = false;
    ev.x = -1;
    ev.y = -1;
    ev.action = TOUCH_NONE;
    if (!_tft) return ev;

    uint16_t tx=0, ty=0;
    bool touched = _tft->getTouch(&tx, &ty, 400);
    unsigned long now = millis();

    if (touched) {
        ev.pressed = true;
        ev.x = tx;
        ev.y = ty;
        if (!_wasTouching) {
            _wasTouching = true;
            _pressTime = now;
            _pressX = tx;
            _pressY = ty;
            _longReported = false;
        }
        else if (!_longReported && now - _pressTime >= 600) {
            _longReported = true;
            ev.action = TOUCH_LONG_PRESS;
        }
    }
    else if (_wasTouching) {
        _wasTouching = false;
        if (_longReported) {
            ev.action = TOUCH_LONG_PRESS;
        } else {
            ev.action = TOUCH_TAP;
        }
        ev.x = _pressX;
        ev.y = _pressY;
    }
    return ev;
}

bool TouchManager::getTouchRaw(uint16_t *rawX, uint16_t *rawY, uint16_t threshold) {
    if (!_tft) return false;
    
    // Use TFT_eSPI's getTouchRaw which reads raw ADC values without calibration
    // We need to call validTouch + getTouchRaw to get stable raw values
    // But TFT_eSPI doesn't expose validTouch publicly, so we use getTouchRaw directly
    // and do our own debouncing
    
    uint16_t x1, y1, x2, y2;
    
    // Read first sample
    _tft->getTouchRaw(&x1, &y1);
    
    // Check pressure
    uint16_t z = _tft->getTouchRawZ();
    if (z < threshold) return false;
    
    delay(2);
    
    // Read second sample
    _tft->getTouchRaw(&x2, &y2);
    
    // Check pressure again
    z = _tft->getTouchRawZ();
    if (z < threshold) return false;
    
    // Average the two samples
    *rawX = (x1 + x2) / 2;
    *rawY = (y1 + y2) / 2;
    
    return true;
}


#include "TFT_h/touchScreenManager.h"

TouchPoint TouchScreenManager::readRawTouch() {
    uint16_t t_x = 0, t_y = 0;
    bool pressed = tft->getTouch(&t_x, &t_y);
    
    Serial.print(".");
    if (pressed) {
        return TouchPoint(static_cast<int16_t>(t_x), static_cast<int16_t>(t_y), true);
        Serial.printf("Touch at (%d, %d)\n", t_x, t_y);
    }
    
    return TouchPoint();
}

void TouchScreenManager::begin(TFT_eSPI* tft_espi) {
    tft = tft_espi;
}

void TouchScreenManager::update() {
    TouchPoint currentTouch = readRawTouch();
    
    if (!currentTouch.valid && wasTouched) {
        // Touch was released
        justReleased = true;
        wasTouched = false;
    } else if (currentTouch.valid) {
        // Touch is active
        justReleased = false;
        wasTouched = true;
        lastTouch = currentTouch;
    } else {
        justReleased = false;
    }
}

TouchPoint TouchScreenManager::getTouch() {
    if (justReleased) {
        justReleased = false;
        return lastTouch;
    }
    return readRawTouch();
}
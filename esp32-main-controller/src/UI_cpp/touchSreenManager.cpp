#include "Ui_headers/touchScreenManager.h"
#include "config/pinLayout.h"


uint32_t TouchButton::cooldownEnd;
std::function<void()> TouchButton::powerStateTimeResetCb = nullptr;

TouchPoint TouchScreenManager::readRawTouch() {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        return TouchPoint(static_cast<int16_t>(p.x), static_cast<int16_t>(p.y), true);
    }
    return TouchPoint();
}

void TouchScreenManager::begin() {
    pinMode(PinLayout::touchRST, OUTPUT);
    digitalWrite(PinLayout::touchRST, LOW);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(PinLayout::touchRST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));

    pinMode(PinLayout::touchSDA, INPUT_PULLUP);
    pinMode(PinLayout::touchSCL, INPUT_PULLUP);
    Wire.begin(PinLayout::touchSDA, PinLayout::touchSCL);

    ts.begin(40, &Wire);
}

void TouchScreenManager::update() {
    TouchPoint currentTouch = readRawTouch();

    if (!currentTouch.valid && wasTouched) {
        justReleased = true;
        wasTouched = false;
    } else if (currentTouch.valid) {
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

bool TouchScreenManager::touchDetected() {
    return ts.touched();
}
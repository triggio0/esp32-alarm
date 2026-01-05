#pragma once
#include <stdint.h>
#include <functional>
#include <TFT_eSPI.h>

struct TouchPoint {
    int16_t x;
    int16_t y;
    bool valid;

    TouchPoint() : x(0), y(0), valid(false) {}
    TouchPoint(int16_t x, int16_t y, bool valid) : x(x), y(y), valid(valid) {}
};

struct TouchRect {
    int16_t startX;
    int16_t startY;
    int16_t endX;
    int16_t endY;

    TouchRect() : startX(0), startY(0), endX(0), endY(0) {}
    bool containsPoint(TouchPoint touchPoint) {
        if (!touchPoint.valid) return false;
        return (touchPoint.x >= startX && touchPoint.x <= endX &&
                touchPoint.y >= startY && touchPoint.y <= endY);
    }
};

class TouchButton {
private:
    TouchRect boundingBox;
    bool hovering {false};
    std::function<void(bool hovering)> drawCallback;
    std::function<void()> selectionCallback;
    static uint32_t cooldownEnd;
    static std::function<void()> powerStateTimeResetCb;
    
public:
    
    TouchButton() = default;
    TouchButton(TouchRect bounds, std::function<void(bool hovering)> drawCb, std::function<void()> selectionCb) :
        boundingBox(bounds), drawCallback(drawCb), selectionCallback(selectionCb) {}; 
    static void setPowerStateTimeResetCb(std::function<void()> callback) {
        powerStateTimeResetCb = callback;
    }
    bool checkCollision(TouchPoint touchPoint) {
        powerStateTimeResetCb();                        // TODO: check efficiency
        if (millis() < cooldownEnd) {
            return false;
        }
        if (boundingBox.containsPoint(touchPoint)) {
            if (!hovering) {
                drawCallback(true);
            }
            hovering = true;
            return true;
        } else {
            if (hovering) {
                drawCallback(false);
                if (!touchPoint.valid) {
                    selectionCallback();
                    cooldown();
                }
            }
            hovering = false;
            return false;
        }
    };

    void cooldown(uint16_t cooldownMs=100) {
        cooldownEnd = millis() + cooldownMs;
    }
};

class TouchScreenManager {
private:
    TFT_eSPI* tft;
    bool justReleased {false};
    bool wasTouched {false};
    TouchPoint lastTouch;

    uint16_t calibrationData[5] = {325, 3331, 342, 3540, 2};
    

private:
    TouchPoint readRawTouch();
public:
    void begin(TFT_eSPI* tft_espi);
    void update();
    TouchPoint getTouch();
    bool isJustReleased() { return justReleased; }
    void calibrateTouch();
    bool touchDetected();
};
    
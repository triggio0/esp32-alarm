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
public:
    TouchButton() = default;
    TouchButton(TouchRect bounds, std::function<void(bool hovering)> drawCb, std::function<void()> selectionCb) :
        boundingBox(bounds), drawCallback(drawCb), selectionCallback(selectionCb) {}; 

    bool checkCollision(TouchPoint touchPoint) {
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

    void cooldown(uint16_t cooldownMs=200) {
        cooldownEnd = millis() + cooldownMs;
    }
};

// class TouchScreenManager {
// private:
//     bool justReleased {false};
// public:
//     TouchPoint getTouch() { return TouchPoint(); };
//     bool isJustReleased() { return justReleased; };

// };

class TouchScreenManager {
private:
    TFT_eSPI* tft;
    bool justReleased {false};
    bool wasTouched {false};
    TouchPoint lastTouch;

    uint16_t calibrationData[5] = {325, 3331, 342, 3540, 2};    // {291, 3642, 273, 3480, 1}
    

private:
    TouchPoint readRawTouch();
public:
    void begin(TFT_eSPI* tft_espi);
    void update();
    TouchPoint getTouch();
    bool isJustReleased() { return justReleased; }

    void calibrateTouch() {
        if (!tft) return;
        
        uint16_t calData[5];
        
        tft->fillScreen(TFT_BLACK);
        tft->setCursor(20, 0);
        tft->setTextFont(2);
        tft->setTextSize(1);
        tft->setTextColor(TFT_WHITE, TFT_BLACK);
        
        tft->println("Touch corners as indicated");
        tft->setTextFont(1);
        tft->println();
        
        tft->calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
        
        // Print calibration data for future use
        Serial.println("Touch calibration data:");
        Serial.print("uint16_t calData[5] = {");
        for (uint8_t i = 0; i < 5; i++) {
            Serial.print(calData[i]);
            if (i < 4) Serial.print(", ");
        }
        Serial.println("};");
        
        tft->fillScreen(TFT_BLACK);
        tft->setTextColor(TFT_GREEN, TFT_BLACK);
        tft->println("Calibration complete!");
        delay(2000);
    }

};
    
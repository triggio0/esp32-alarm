#pragma once
#include <stdint.h>

struct TouchPoint {
    int16_t x;
    int16_t y;
    bool valid;

    TouchPoint() : x(0), y(0), valid(false) {}
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

class TouchScreenManager {
private:
    bool justReleased;
public:
    TouchPoint getTouch();
    bool isJustReleased();

};
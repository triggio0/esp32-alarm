#pragma once
#include <stdint.h>
#include <functional>

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

class TouchButton {
private:
    TouchRect boundingBox;
    bool hovering {false};
    std::function<void(bool hovering)> drawCallback;
    std::function<void()> selectionCallback;
public:
    TouchButton() = default;
    TouchButton(TouchRect bounds, std::function<void(bool hovering)> drawCb, std::function<void()> selectionCb) :
        boundingBox(bounds), drawCallback(drawCb), selectionCallback(selectionCb) {}; 

    bool checkCollision(TouchPoint touchPoint) {
        if (boundingBox.containsPoint(touchPoint)) {
            if (!hovering) {
                drawCallback(true);
            }
            hovering = true;
            return true;
        } else {
            if (hovering) {
                drawCallback(false);
            }
            hovering = false;
            return false;
        }
    }
};

class TouchScreenManager {
private:
    bool justReleased {false};
public:
    TouchPoint getTouch() { return TouchPoint(); }  // TODO: placeholder
    bool isJustReleased() { return justReleased; };

};
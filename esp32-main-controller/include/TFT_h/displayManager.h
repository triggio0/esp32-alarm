#pragma once
#include <TFT_eSPI.h>
#include "utils.h"

class DisplayManager {
    /*
    **  only need to .begin()
    */
private:
    TFT_eSPI tft = TFT_eSPI();
    bool initialized {false};
    static constexpr int8_t screenRotation {2};     //pins at the top

    static constexpr int16_t pwmFreq {5000};
    static constexpr int16_t pwmResolution {8};

public:
    void begin();
    void fillColor(uint16_t color = TFT_BLACK);
    int16_t width();
    int16_t height();
    TFT_eSprite* createSprite(int16_t w, int16_t h);
    void deleteSprite(TFT_eSprite* sprite);
    TFT_eSPI* getTFT();
    void setBacklight(int16_t brightnessPercentage);
    void printFps(float fps);
    void printFps();
};
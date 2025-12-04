#pragma once
#include <TFT_eSPI.h>



class DisplayManager {
    /*
    **  only need to .begin()
    */
private:
    TFT_eSPI tft = TFT_eSPI();
    bool initialized {false};

public:
    void begin();
    void fillColor(uint16_t color = TFT_BLACK);
    int16_t width();
    int16_t height();
    TFT_eSprite* createSprite(int16_t w, int16_t h);
    void deleteSprite(TFT_eSprite* sprite);
    TFT_eSPI* getTFT();
};
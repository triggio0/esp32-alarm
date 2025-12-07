#include "graphicsH/displayManager.h"
#include "config/pinLayout.h"

void DisplayManager::begin() {
    if (initialized) return;
    tft.init();
    tft.setRotation(screenRotation);
    fillColor();

    ledcSetup(PinLayout::backlightChannel, pwmFreq, pwmResolution);
    ledcAttachPin(PinLayout::tftBacklight, PinLayout::backlightChannel);
    setBacklight(100);

    initialized = true;
    Serial.println("Display initialized!");
}

void DisplayManager::setBacklight(int16_t brightnessPercentage) {
    int dutyCycle = (brightnessPercentage * ((1 << pwmResolution) - 1)) / 100;
    ledcWrite(PinLayout::backlightChannel, dutyCycle);
}

void DisplayManager::fillColor(uint16_t color) {
    tft.fillScreen(color);
}

int16_t DisplayManager::width() {
    return tft.width();
}

int16_t DisplayManager::height() {
    return tft.height();
}

TFT_eSprite* DisplayManager::createSprite(int16_t w, int16_t h) {
    TFT_eSprite* sprite = new TFT_eSprite(&tft);
    sprite->createSprite(w, h);
    return sprite;
}

void DisplayManager::deleteSprite(TFT_eSprite* sprite) {
    if (sprite) {
        sprite->deleteSprite();
        delete sprite;
    }
}

TFT_eSPI* DisplayManager::getTFT() {
    return &tft;
}

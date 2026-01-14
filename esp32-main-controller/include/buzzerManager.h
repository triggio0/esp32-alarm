#pragma once

#include "config/pinLayout.h"
#include <Arduino.h>

class BuzzerManager {
private:
    static constexpr int16_t buzzerPin {PinLayout::buzzer};
    static constexpr int16_t buzzerChannel {PinLayout::buzzerChannel};
    static constexpr int16_t buzzerResolution = 8;
    
    uint32_t beepEndTime = 0;
    bool isBeeping = false;


public:

    BuzzerManager() {
        ledcSetup(buzzerChannel, 2000, buzzerResolution);  // 2000 Hz, 8-bit
        ledcAttachPin(buzzerPin, buzzerChannel);
        ledcWrite(buzzerChannel, 0);  // Start silent
    }
    
    void beep(uint16_t durationMs, uint16_t frequency = 2000, uint8_t volume = 128) {
        ledcSetup(buzzerChannel, frequency, buzzerResolution);
        ledcWrite(buzzerChannel, volume);  // 0-255 duty cycle
        
        beepEndTime = millis() + durationMs;
        isBeeping = true;
    }    

    
    void stop() {
        ledcWrite(buzzerChannel, 0);
        isBeeping = false;
    }
    
    void update() {
        if (isBeeping && millis() >= beepEndTime) {
            stop();
        }
    }
    
    bool isBuzzing() const {
        return isBeeping;
    }
};
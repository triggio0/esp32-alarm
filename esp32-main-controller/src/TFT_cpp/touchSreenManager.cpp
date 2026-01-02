#include "TFT_h/touchScreenManager.h"

uint32_t TouchButton::cooldownEnd; 

TouchPoint TouchScreenManager::readRawTouch() {
    uint16_t t_x = 0, t_y = 0;
    bool pressed = tft->getTouch(&t_x, &t_y);
    if (pressed) {
        return TouchPoint(static_cast<int16_t>(t_x), static_cast<int16_t>(t_y), true); 
    }
    return TouchPoint();
}

void TouchScreenManager::begin(TFT_eSPI* tft_espi) {
    tft = tft_espi;
    if (calibrationData[0] == 0 && calibrationData[1] == 0 &&
        calibrationData[2] == 0 && calibrationData[3] == 0 &&
        calibrationData[4] == 0) {
        tft->calibrateTouch(calibrationData, TFT_MAGENTA, TFT_BLACK, 15);
        Serial.print("Touch calibration data: {");
        for (uint8_t i = 0; i < 5; i++) {
            Serial.print(calibrationData[i]);
            if (i < 4) Serial.print(", ");
        }
        Serial.println("}");
        tft->fillScreen(TFT_WHITE);
    }
    tft->setTouch(calibrationData);
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

bool TouchScreenManager::touchDetected() {
    uint16_t t_x = 0, t_y = 0;
    return (tft->getTouch(&t_x, &t_y));
}

void TouchScreenManager::calibrateTouch() {
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
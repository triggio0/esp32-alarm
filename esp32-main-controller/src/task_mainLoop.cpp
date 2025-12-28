#include <Arduino.h>
#include "TFT_h/UiManager.h"

UiManager uiManager;

void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");

    uiManager.begin();

    uint32_t lastFrameTime = millis();

    while (true) {
        uint32_t now = millis();
        float fps = 1000.0f / (now - lastFrameTime);
        lastFrameTime = now;

        auto* tft = uiManager.getTFT();
        tft->setTextColor(TFT_BLACK, TFT_WHITE);
        tft->setCursor(2, 2);
        tft->setTextSize(2);
        tft->printf("%.1f", fps);

        uiManager.update();

        delay(10);
    }
}

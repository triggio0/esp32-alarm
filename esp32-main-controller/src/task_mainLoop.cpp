#include <Arduino.h>
#include "Ui_headers/UiManager.h"
#include "Telegram.h"

constexpr int MAX_FRAMERATE {16};


UiManager uiManager;
TelegramBot tgBot;

void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");
    uiManager.begin();
    tgBot = TelegramBot();
    uiManager.setFpsOptimizationCb(
        []() { tgBot.setOff(); Serial.println("efficient!"); },
        []() { tgBot.setOn(); }
    );

    const TickType_t frameDelay = pdMS_TO_TICKS( static_cast<int>( 1000 / MAX_FRAMERATE ) );
    TickType_t lastWakeTime = xTaskGetTickCount();

    Serial.printf("used heap: %.1f%%\n", getHeapUsedPercent());

    int count {0};
    while (true) {
        uiManager.update();
        tgBot.update();

        count++;
        if (count == 500) {
            Serial.printf("- %.0f\n", getFpsCount());
            count = 0;
        } else {
            getFpsCount();
        }
        vTaskDelayUntil(&lastWakeTime, frameDelay);
    }
}
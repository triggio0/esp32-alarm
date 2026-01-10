#include <Arduino.h>
#include "TFT_h/UiManager.h"

constexpr int MAX_FRAMERATE {16};


UiManager uiManager;

void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");
    uiManager.begin();

    const TickType_t frameDelay = pdMS_TO_TICKS( static_cast<int>( 1000 / MAX_FRAMERATE ) );
    TickType_t lastWakeTime = xTaskGetTickCount();

    Serial.printf("used heap: %.1f%%\n", getHeapUsedPercent());

    int count {0};
    while (true) {
        uiManager.update();
        // vTaskDelayUntil(&lastWakeTime, frameDelay);

        count++;
        if (count == 15) {
            Serial.printf("- %.0f\n", getFpsCount());
            count = 0;
        } else {
            getFpsCount();
        }
    }
}
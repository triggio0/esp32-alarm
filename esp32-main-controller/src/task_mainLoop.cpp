#include <Arduino.h>
#include "TFT_h/UiManager.h"

UiManager uiManager;

void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");

    uiManager.begin();

    while (true) {
        

        uiManager.update();

        delay(10);
    }
}

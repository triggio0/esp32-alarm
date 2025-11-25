#include "toInclude.h"
#include "graphics.h"

DisplayManager displayManager;
EyeSprite eyeSprite;
KeypadGraph keypadGraph;
AlarmState currentState {disarmed};


float getHeapUsedPercent() {
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    if (total == 0) return 0.0f;                 // defensive
    size_t freeBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t used = total - freeBytes;
    return (float)used * 100.0f / (float)total;
}



void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("Starting...");

    randomSeed(analogRead(A0));
    pinMode(0, INPUT_PULLUP);
    
    displayManager.begin();
    displayManager.fillColor(TFT_WHITE);


    eyeSprite.begin(&displayManager);
    eyeSprite.setAlarmState(currentState);

    keypadGraph.begin(&displayManager);
    keypadGraph.pushAll();

    keypadGraph.selectKey('3');

    
    getHeapUsedPercent();
    Serial.println(getHeapUsedPercent());
}

void loop() {


    eyeSprite.update();
    eyeSprite.push();
    

    if (digitalRead(0) == LOW) {
        if (currentState == disarmed) currentState = armedAway;
        else if (currentState == armedAway) currentState = soundAlarm;
        else if (currentState == soundAlarm) currentState = disarmed;
        eyeSprite.setAlarmState(currentState);
        Serial.println(currentState);
        delay(100);
    }

    delay(16);
}
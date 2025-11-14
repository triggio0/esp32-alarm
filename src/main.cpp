#include "toInclude.h"
#include "graphics.h"

DisplayManager displayManager;
EyeSprite eyeSprite;
AlarmState currentState {disarmed};


void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("Starting...");

    randomSeed(analogRead(A0));
    pinMode(0, INPUT_PULLUP);
    
    displayManager.begin();
    eyeSprite.begin(&displayManager);

    displayManager.fillColor(TFT_BLACK);

    eyeSprite.setAlarmState(currentState);
}

void loop() {


    eyeSprite.update();
    eyeSprite.push();
    delay(16);

    if (digitalRead(0) == LOW) {
        if (currentState == disarmed) currentState = armedAway;
        else if (currentState == armedAway) currentState = soundAlarm;
        else if (currentState == soundAlarm) currentState = disarmed;
        eyeSprite.setAlarmState(currentState);
        Serial.println(currentState);
        delay(100);
    }

    
}
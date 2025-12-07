#include "toInclude.h"
#include "graphicsH/displayManager.h"
#include "graphicsH/eye.h"
#include "graphicsH/keypad.h"
#include "graphicsH/statusScreen.h"
#include "utils.h"

DisplayManager displayManager;
EyeSprite eyeSprite;
KeypadGraph keypadGraph;
MainMenuGraph mainMenuGraph;
AlarmState currentState {disarmed};



void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("Starting...");

    randomSeed(analogRead(A0));
    pinMode(0, INPUT_PULLUP);

    displayManager.begin();
    displayManager.fillColor(TFT_WHITE);


    eyeSprite.begin(&displayManager);

    keypadGraph.begin(&displayManager);
    keypadGraph.pushAll();
    keypadGraph.selectKey('3');
    keypadGraph.setBufferedNumbers(3);

    // mainMenuGraph.begin(&displayManager);
    // mainMenuGraph.setEntryBool(camera, true);
    // mainMenuGraph.pushAll();


    getHeapUsedPercent();
    Serial.println(getHeapUsedPercent());
}

void loop() {

    eyeSprite.update();
    eyeSprite.push();

    delay(16);
}

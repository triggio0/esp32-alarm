#include "toInclude.h"
#include "graphicsH/displayManager.h"
#include "graphicsH/eye.h"
#include "graphicsH/keypad.h"
#include "graphicsH/statusScreen.h"

DisplayManager displayManager;
EyeSprite eyeSprite;
KeypadGraph keypadGraph;
MainMenuGraph mainMenuGraph;
AlarmState currentState {disarmed};

float getHeapUsedPercent() {
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    if (total == 0) return 0.0f;
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

#include "toInclude.h"
#include "TFT_h/displayManager.h"
#include "TFT_h/statusScreen.h"
#include "TFT_h/touchScreenManager.h"
#include "TFT_h/keypad.h"
#include "TFT_h/eye.h"

DisplayManager displayManager;
MainMenuGraph mainMenuGraph;
KeypadGraph keypadGraph;
EyeSprite eyeSprite;
TouchScreenManager touchScreenManager;








void manageState() {
    
    eyeSprite.update();
    eyeSprite.push();

    return;
}















/*
void disarmedLoop() {
    
    mainMenuGraph.pushAll();

    while (true) {
        eyeSprite.push();
        eyeSprite.update();
        mainMenuGraph.update();


        if (currentAlarmState != disarmed) break;
        delay(10);
    }
}

void armedHomeLoop() {

    mainMenuGraph.pushAll();

    while (true) {
        eyeSprite.push();
        eyeSprite.update();
        mainMenuGraph.update();

        // TODO: read sensors

        if (currentAlarmState != armedHome) break;
        delay(10);
    }
}

void manageState() {

    mainMenuGraph.setAlarmState(currentAlarmState);

    switch (currentAlarmState)
    {
    case disarmed:
        disarmedLoop();
        break;
    case armedHome:
        armedHomeLoop();
        break;
    case armedAway:
        armedAwayLoop();
        break;
    case soundAlarm:
        soundAlarmLoop();
        break;
    case lockdown:
        lockdownLoop();
        break;
    default:
        break;
    }
}
*/

void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");

    // begin 
    displayManager.begin();
    touchScreenManager = TouchScreenManager();
    keypadGraph.begin(&displayManager, &touchScreenManager);
    mainMenuGraph.begin(&displayManager, &touchScreenManager);
    eyeSprite.begin(&displayManager);



    while (true) {

        manageState();

        delay(10);
    }
}
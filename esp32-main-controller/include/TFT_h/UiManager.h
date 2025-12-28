#pragma once
#include "displayManager.h"
#include "touchScreenManager.h"
#include "statusScreen.h"
#include "keypad.h"
#include "eye.h"



class UiManager {
private:

    DisplayManager displayManager;
    MainMenuGraph mainMenuGraph;
    KeypadGraph keypadGraph;
    EyeSprite eyeSprite;
    TouchScreenManager touchScreenManager;



public:


    void begin() {
        displayManager.begin();
        displayManager.fillColor(TFT_WHITE);
        touchScreenManager.begin(displayManager.getTFT());
        keypadGraph.begin(&displayManager, &touchScreenManager);
        mainMenuGraph.begin(&displayManager, &touchScreenManager);
        mainMenuGraph.setAlarmState(alarmState);
        mainMenuGraph.pushAll();
        eyeSprite.begin(&displayManager);
        eyeSprite.push(true);

    }


    void update() {

        eyeSprite.update();
        eyeSprite.push();

        


    }



};
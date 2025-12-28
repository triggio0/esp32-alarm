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

    bool disarmSequence {false};



    void updateActive() {
        eyeSprite.update();
        eyeSprite.push();

        if (!disarmSequence) mainMenuGraph.update();
        else keypadGraph.update();


    }




public:


    void begin() {
        displayManager.begin();
        displayManager.fillColor(TFT_WHITE);
        touchScreenManager.begin(displayManager.getTFT());
        keypadGraph.begin(&displayManager, &touchScreenManager);
        mainMenuGraph.begin(&displayManager, &touchScreenManager,
            [this](){ keypadGraph.startUnlockSequence(); disarmSequence = true; });
        mainMenuGraph.pushAll();
        eyeSprite.begin(&displayManager);
        eyeSprite.push(true);

    }


    void update() {

        updateActive();

    }

    TFT_eSPI* getTFT() { return displayManager.getTFT(); }

};
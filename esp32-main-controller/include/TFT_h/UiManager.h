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

    uint32_t currentTime {};
    bool needFullPush {true};

    static constexpr uint16_t idleDimTimeout {30 * 1000};
    static constexpr uint16_t powerSavingTimeout {60 * 1000};
    PowerState powerState {active};
    uint32_t startPowerStateTime {};


    static constexpr uint16_t disarmSequenceDuration {};
    static constexpr uint8_t maxCodeRetries {3};
    bool disarmSequence {false};
    uint32_t disarmSequenceStart {};
    uint8_t currentCodeRetries {};



    void codeFailed() {                                   // TODO
        if (System::alarmState == armedAway) {

        } else if (System::alarmState == armedHome) {

        }
    }

    void disarmSequenceUpdate() {

        if (disarmSequenceStart + disarmSequenceDuration >= millis()) {
            codeFailed();
        }
        keypadGraph.update();
        if (keypadGraph.getUnlockSequenceState() == inProgress) {
            return;
        } else if (keypadGraph.getUnlockSequenceState() == codeSuccess) {
            disarmSequence = false;
            if (System::alarmState == armedAway || System::alarmState == armedHome) {
                System::alarmState = disarmed;
            }
        } else if (keypadGraph.getUnlockSequenceState() == codeAbort) {
            disarmSequence = false;
        } else if (keypadGraph.getUnlockSequenceState() == codeFail) {
            currentCodeRetries++;
            if (currentCodeRetries > maxCodeRetries) {
                currentCodeRetries = 0;
                codeFailed();
            }
            else {
                keypadGraph.setUnlockSequenceState();
                return;
            }
        }

        pushAll();
    }

    void pushAll() {
        displayManager.fillColor(TFT_WHITE);
        eyeSprite.push(true);
        if (disarmSequence) {
            keypadGraph.pushAll();
        } else {
            mainMenuGraph.pushAll();
        }
    }

    void updateActive() {
        eyeSprite.update();
        eyeSprite.push();

        if (disarmSequence) {
            disarmSequenceUpdate();
        } else {
            mainMenuGraph.update();
        }
    }

    void updateLowPowerMode() {
        if(touchScreenManager.touchDetected()) {
            powerState = active;
            displayManager.setBacklight(100);
        }

    }



public:
    void begin() {
        displayManager.begin();
        displayManager.fillColor(TFT_WHITE);
        touchScreenManager.begin(displayManager.getTFT());
        TouchButton::setPowerStateTimeResetCb([this]() {
            startPowerStateTime = currentTime;
            if (powerState == idleDim || powerState == powerSaving)
            powerState = active;
            displayManager.setBacklight(100);
        });
        keypadGraph.begin(&displayManager, &touchScreenManager);
        mainMenuGraph.begin(&displayManager, &touchScreenManager,
            [this]() {
                keypadGraph.setUnlockSequenceState();
                keypadGraph.pushAll();
                disarmSequence = true;
                disarmSequenceStart = millis();
            });
        eyeSprite.begin(&displayManager);
        pushAll();

        startPowerStateTime = millis();
    }


    void update() {
        currentTime = millis();
        displayManager.printFps();

        if (currentTime > startPowerStateTime + powerSavingTimeout) {
            if (powerState != powerSaving) displayManager.setBacklight(0);
            updateLowPowerMode();
        } else if (currentTime > startPowerStateTime + idleDimTimeout) {
            if (powerState != idleDim) displayManager.setBacklight(40);
            updateLowPowerMode();
        } else {
            updateActive();
        }

        
    }

    TFT_eSPI* getTFT() { return displayManager.getTFT(); }

};
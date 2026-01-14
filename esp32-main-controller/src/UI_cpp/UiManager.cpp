#include "Ui_headers/UiManager.h"

// void UiManager::codeFailed() {                                   // TODO
//         if (System::alarmState == armedAway) {

//         } else if (System::alarmState == armedHome) {

//         }
//     }

void UiManager::disarmSequenceUpdate() {                        // TODO: auto exit (blocks telegram)

    if (disarmSequenceStart + disarmSequenceDuration >= millis()) {
        alarmTrigger();
    }
    keypadGraph.update();
    if (keypadGraph.getUnlockSequenceState() == inProgress) {
        return;
    } else if (keypadGraph.getUnlockSequenceState() == codeSuccess) {
        endDisarmSequence();
        if (System::alarmState == armedAway || System::alarmState == armedHome) {
            System::alarmState = disarmed;
        }
    } else if (keypadGraph.getUnlockSequenceState() == codeAbort) {
        endDisarmSequence();
    } else if (keypadGraph.getUnlockSequenceState() == codeFail) {
        currentCodeRetries++;
        if (currentCodeRetries > maxCodeRetries) {
            currentCodeRetries = 0;
            alarmTrigger();
        }
        else {
            keypadGraph.setUnlockSequenceState();
            return;
        }
    }
    pushAll();
}

void UiManager::pushAll() {
    displayManager.fillColor(TFT_WHITE);
    eyeSprite.push(true);
    if (disarmSequence) {
        keypadGraph.pushAll();
    } else {
        mainMenuGraph.pushAll();
    }
}

void UiManager::updateActive() {
    eyeSprite.update();
    eyeSprite.push();

    if (disarmSequence) {
        disarmSequenceUpdate();
    } else {
        mainMenuGraph.update();
    }
}

void UiManager::updateLowPowerMode() {
    if(touchScreenManager.touchDetected()) {
        powerState = active;
        displayManager.setBacklight(100);
        startPowerStateTime = currentTime;
    }
}

void UiManager::startDisarmSequence() {
    disarmSequence = true;
    if (startOptimization) startOptimization();
}

void UiManager::endDisarmSequence() {
    disarmSequence = false;
    if (endOptimization) endOptimization();
}

void UiManager::begin() {
    displayManager.begin();
    displayManager.fillColor(TFT_WHITE);
    touchScreenManager.begin(displayManager.getTFT());

    TouchButton::setPowerStateTimeResetCb([this]() {
        startPowerStateTime = currentTime;
    });

    keypadGraph.begin(&displayManager, &touchScreenManager);
    mainMenuGraph.begin(&displayManager, &touchScreenManager, [this]() {
        keypadGraph.setUnlockSequenceState();
        keypadGraph.pushAll();
        startDisarmSequence();
        disarmSequenceStart = millis();
    });
    
    eyeSprite.begin(&displayManager);
    pushAll();

    startPowerStateTime = millis();
}


void UiManager::update() {
    currentTime = millis();

    if (currentTime > startPowerStateTime + powerSavingTimeout) {
        if (powerState != powerSaving) displayManager.setBacklight(0);
        powerState = powerSaving;
        updateLowPowerMode();
    } else if (currentTime > startPowerStateTime + idleDimTimeout) {
        if (powerState != idleDim) displayManager.setBacklight(40);
        powerState = idleDim;
        updateLowPowerMode();
    } else {
        updateActive();
    }   
}
#pragma once
#include "displayManager.h"
#include "touchScreenManager.h"
#include "statusScreen.h"
#include "keypad.h"
#include "eye.h"
#include "buzzerManager.h"


class UiManager {
private:
    DisplayManager displayManager;
    MainMenuGraph mainMenuGraph;
    KeypadGraph keypadGraph;
    EyeSprite eyeSprite;
    TouchScreenManager touchScreenManager;
    // BuzzerManager* buzzerManager;

    uint32_t currentTime {};
    bool needFullPush {true};

    std::function<void()> startOptimization;
    std::function<void()> endOptimization;
    std::function<void()> alarmTrigger;

    static constexpr uint32_t idleDimTimeout {60 * 60 * 1000};
    static constexpr uint32_t powerSavingTimeout {10 * 1000 + idleDimTimeout};
    PowerState powerState {active};
    uint32_t startPowerStateTime {};

    static constexpr uint8_t maxCodeRetries {3};
    bool disarmSequence {false};
    uint8_t currentCodeRetries {};

    void codeFailed();              // TODO remove?
    void disarmSequenceUpdate();
    void pushAll();
    void updateActive();
    void updateLowPowerMode();
    void startDisarmSequence();
    void endDisarmSequence();

public:
    void begin();
    void update();
    void setFpsOptimizationCb(std::function<void()> startOptimization, std::function<void()> endOptimization) {
        UiManager::startOptimization = startOptimization;
        UiManager::endOptimization = endOptimization;
    }
    void setAlarmTriggerCb(std::function<void()> alarmTriggerCb) { alarmTrigger = alarmTriggerCb; }         // TODO
    // void setBuzzerManager(BuzzerManager* buzzerMng) { buzzerManager = buzzerMng; }
    TFT_eSPI* getTFT() { return displayManager.getTFT(); }

};
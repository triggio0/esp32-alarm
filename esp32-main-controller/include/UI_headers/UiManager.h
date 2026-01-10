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

    void codeFailed();
    void disarmSequenceUpdate();
    void pushAll();
    void updateActive();
    void updateLowPowerMode();

public:
    void begin();
    void update();
    TFT_eSPI* getTFT() { return displayManager.getTFT(); }

};
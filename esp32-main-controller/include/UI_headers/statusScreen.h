#pragma once
#include <unordered_map>
#include "globals.h"
#include "displayManager.h"
#include "touchScreenManager.h"

enum StatusScreenEntries {
    front_door,
    window,
    window_sensor,
    camera,
    wifi,
};

class StatusEntry {
public:
    static TFT_eSPI* tft;
    static int16_t firstLinePosX;
    static int16_t firstLinePosY;
    static int16_t lineHeight;
    static int16_t lineWidth;
    static int16_t textSize;
    static int16_t textFont;
    static int16_t textIndentation;
    static uint16_t textColor;
    static uint16_t bgColor;
    static uint16_t trueColor;
    static uint16_t falseColor;

private:
    StatusScreenEntries entryID;
    char displayedEntry[15];
    char displayedValueTrue[10];
    char displayedValueFalse[10];

    bool value;
    int16_t line;

    void pushRightSide();
public:
    StatusEntry() : entryID(), line(0), displayedEntry{0}, displayedValueTrue{0}, displayedValueFalse{0}, value(false) {}
    StatusEntry(StatusScreenEntries id, int16_t lineNumber, const char* entryName,
        const char* valueTrue, const char* valueFalse);
    StatusScreenEntries getEntryID() const {return entryID;}
    void setValue(bool b);
    void push();
};



class MainMenuGraph {
private:
    DisplayManager* displayManager;
    TouchScreenManager* tsManager;
    TFT_eSPI* tft;

    static constexpr int16_t posX {5};
    static constexpr int16_t posY {215};
    static constexpr int16_t width {310};
    static constexpr int16_t height {260};
    static constexpr int16_t headerHeight {25};
    static constexpr int16_t lineHeight {26};

    static constexpr uint16_t bgColor {TFT_WHITE};
    static constexpr uint16_t textColor {TFT_BLACK};
    static constexpr uint16_t trueColor {TFT_GREEN};
    static constexpr uint16_t falseColor {TFT_RED};
    static constexpr uint16_t buttonColor {TFT_LIGHTGREY};
    static constexpr uint16_t buttonColorPressed {TFT_DARKGREY};

    static constexpr uint8_t numEntries {5};
    StatusEntry entries[numEntries];
    static constexpr int16_t textSize {2};
    static constexpr int16_t textFont {1};
    static constexpr int16_t textIndentation {6};

    static const std::unordered_map<AlarmState, const char*> stateNames;
    static const std::unordered_map<AlarmState, const char*> actionNames;
    static constexpr int16_t buttonNegY {40};           // measured from the bottom (center of rect)
    static constexpr int16_t buttonHeight {55};
    static constexpr int16_t buttonWidth {150};
    static constexpr int16_t buttonCornerR {5};

    static constexpr int16_t largeButtonHeight_submenu {80};
    static constexpr int16_t largeButtonWidth_submenu {260};
    static constexpr int16_t largeButtonCornerR_submenu {5};
    static constexpr int16_t smallButtonHeight_submenu {55};
    static constexpr int16_t interButtonSpacing_submenu {15};
    static constexpr int16_t textFont_submenu {1};
    static constexpr int16_t textSize_submenu {3};

    TouchButton armDisarmButton;
    TouchButton armedHomeButton;
    TouchButton armedAwayButton;
    TouchButton backButton;

    bool armDisarmButtonIsPressed {false};
    bool armedHomeIsPressed {false};
    bool armedAwayIsPressed {false};
    bool backIsPressed {false};

    bool isInSubmenu {false};
    std::function<void()> startUnlockSequenceCb;

    void pushArmDisarmButton();
    void pushArmDisarmButton(bool hovering);
    void pushHeader();
    void pushArmedHomeButton();
    void pushArmedHomeButton(bool hovering);
    void pushArmedAwayButton();
    void pushArmedAwayButton(bool hovering);
    void pushBackButton();
    void pushBackButton(bool hovering);
    void setAllButtonsFalse();
    void enterSubmenu();
    void exitSubmenu();

public:
    void begin(DisplayManager* dM, TouchScreenManager* tsM, std::function<void()> startUnlockSequenceCallback);
    void update();
    void setEntryBool(StatusScreenEntries entry, bool b);
    void pushAll();
    
};
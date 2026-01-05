#pragma once
#include "displayManager.h"
#include "touchScreenManager.h"
#include "config/secret.h"
#include "globals.h"

class KeypadKey {
public:
    static TFT_eSPI* tft;
    static int16_t width;
    static int16_t height;
    static int16_t cornerRadius;
    static uint16_t bgColor;
    static uint16_t bgSelectColor;
    static uint16_t charColor;

private:
    static constexpr uint8_t textSize {1};
    static constexpr uint8_t textFont {4};
    char ch {' '};
    bool selected = {false};
    int16_t posX {};
    int16_t posY {};

public:
    KeypadKey() = default;
    KeypadKey(char ch, int16_t posX, int16_t posY);
    // void setup(int16_t width, int16_t height, int16_t cornerRadius, uint16_t bgColor, uint16_t bgSelectColor, uint16_t charColor, TFT_eSPI* tft);
    void push();
    void select(bool s);
    char getChar() { return ch; }
};


class KeypadGraph {

private:
    DisplayManager* displayManager;
    TouchScreenManager* tsManager;
    TFT_eSPI* tft;

    uint32_t startTime {};

    static constexpr int16_t posX                           {5};
    static constexpr int16_t posY                           {180};
    static constexpr int16_t width                          {310};
    static constexpr int16_t height                         {290};
    static constexpr int16_t cornerRadius                   {3};
    static constexpr int16_t keySidePaddingX                {5};                // half of the space between keys
    static constexpr int16_t keySidePaddingY                {5};
    static constexpr int16_t pinProgressHeight              {40};
    static constexpr int16_t pinProgressWidth               {160};
    static constexpr int16_t pinProgressCircleRadius        {8};                // also used for width of rectangle
    static constexpr int16_t pinProgressRectHeight          {4};

    static constexpr uint16_t keyBgColor                    {TFT_LIGHTGREY};
    static constexpr uint16_t keyBgSelectColor              {TFT_DARKGREY};
    static constexpr uint16_t charColor                     {TFT_BLACK};
    static constexpr uint16_t bgColor                       {TFT_WHITE};
    static constexpr uint16_t charColorIncorrect            {TFT_RED};

    const char allChars[13] {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', '<', '\0'};
    KeypadKey keyArray[12];
    TouchButton touchButtonArray[12];
    char pwBuffer[6] {};
    UnlockSequenceState unlockSequenceState {none};

    uint8_t bufferedNumbers;
    uint8_t previousBufferedNumbers {};

    void pushBufferedNumber(int16_t position, bool selected, bool incorrect=false);
    void pushBufferedNumbersDisplay();
    void checkCodeBuffer();
    void pushIncorrectNumbersDisplay();
    
public:
    void begin(DisplayManager* displayManager, TouchScreenManager* tsManager);
    void pushAll();
    void update();
    void setUnlockSequenceState(UnlockSequenceState state=inProgress) { unlockSequenceState = state; }
    UnlockSequenceState getUnlockSequenceState() { return unlockSequenceState; }
};
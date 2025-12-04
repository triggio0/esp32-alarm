#pragma once
#include "displayManager.h"

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
    void select (bool s);
};


class KeypadGraph {
private:
    DisplayManager* displayManager;
    TFT_eSPI* tft;

    bool initialized = {false};

    uint32_t startTime {};

    static constexpr int16_t posX {5};
    static constexpr int16_t posY {180};
    static constexpr int16_t width {310};
    static constexpr int16_t height {290};
    static constexpr int16_t cornerRadius {3};
    static constexpr int16_t keySidePaddingX {5};               // half of the space between keys
    static constexpr int16_t keySidePaddingY {5};
    static constexpr int16_t pinProgressHeight {40};
    static constexpr int16_t pinProgressWidth {160};
    static constexpr int16_t pinProgressCircleRadius {8};       // also used for width of rectangle
    static constexpr int16_t pinProgressRectHeight {4};

    static constexpr uint16_t keyBgColor {TFT_LIGHTGREY};
    static constexpr uint16_t keyBgSelectColor {TFT_DARKGREY};
    static constexpr uint16_t charColor {TFT_BLACK};
    static constexpr uint16_t bgColor {TFT_WHITE};

    const char allChars[13] {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', '<', '\0'};
    KeypadKey keyArray[12];

    int16_t previousSelectedNumbers {};

    void pushBufferedNumber(int16_t position, bool selected);

public:
    void begin(DisplayManager* displayManager);
    void selectKey(char ch);
    void deselectKey(char ch);
    void pushAll();
    void setBufferedNumbers(int16_t numberOfBufferedNumbers);
};
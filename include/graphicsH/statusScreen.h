#pragma once
#include "displayManager.h"

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


class StatusScreenGraph {
private:
    DisplayManager* displayManager;
    TFT_eSPI* tft;
    static constexpr uint8_t numEntries {5};
    StatusEntry entries[numEntries];

    static constexpr int16_t posX {5};
    static constexpr int16_t posY {180};
    static constexpr int16_t width {310};
    static constexpr int16_t height {290};
    static constexpr int16_t headerHeight {30};
    static constexpr int16_t lineHeight {30};

    static constexpr uint16_t bgColor {TFT_WHITE};
    static constexpr uint16_t textColor {TFT_BLACK};
    static constexpr uint16_t trueColor {TFT_GREEN};
    static constexpr uint16_t falseColor {TFT_RED};

    static constexpr int16_t textSize {2};       // minecraft style
    static constexpr int16_t textFont {1};

    // static constexpr int16_t textSize {1};
    // static constexpr int16_t textFont {5};
    static constexpr int16_t textIndentation {6};

public:
    void begin(DisplayManager* dM);
    void setEntryBool(StatusScreenEntries entry, bool b);
    void pushAll();
};
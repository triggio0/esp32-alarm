#pragma once
#include <TFT_eSPI.h>
#include "toInclude.h"

constexpr uint16_t transparentColor {0xABCD};

class DisplayManager {
    /*
    **  only need to .begin()
    */
private:
    TFT_eSPI tft = TFT_eSPI();
    bool initialized {false};

public:
    void begin();
    void fillColor(uint16_t color = TFT_BLACK);
    int16_t width();
    int16_t height();
    TFT_eSprite* createSprite(int16_t w, int16_t h);
    void deleteSprite(TFT_eSprite* sprite);
    TFT_eSPI* getTFT();
};

class EyeSprite {
    /*
    **  setup: .begin()
    **  loop: update(), push()
    **  remember to update the state with setAlarmState()
    */
private:

    // ===== Sprites =====
    DisplayManager* displayManager;
    TFT_eSprite* sumSprite;

    // ===== General Stuff =====
    
    uint32_t startTime {};
    AlarmState alarmState {disarmed};
    enum IrisPosition {left, center, right,};
    bool initialized {false};

    // ===== Configuration Constants =====
    static constexpr int16_t posX                       {40};               // wrt screen
    static constexpr int16_t posY                       {100};              // wrt screen
    static constexpr int16_t eyeWidth                   {240};              // determines bezier curve width
    static constexpr float topPMaxHeight                {0.48};             // control point for bezier curve generation
    static constexpr float bottomPMaxHeight             {0.41};             // control point for bezier curve generation
    static constexpr float dozyMinuend                  {0.1};              // minimum 0.05
    static constexpr uint8_t baseAperture               {90};
    static constexpr float tIncrease                    {0.003};            // decrese to avoid zebra stripes on lids
    static constexpr uint16_t blinkClosingTime          {70};
    static constexpr uint16_t blinkOpeningTime          {130};
    static constexpr int16_t irisRadius                 {38};
    static constexpr float irisPosYcLoweringCoeff       {0.03};             // makes iris lower ( * eyeWidth )
    static constexpr float irisRLDistanceCoeff          {0.22};             // 0 ~ 0.5, how far to the side it can look
    static constexpr float irisVelocity                 {0.5};
    static constexpr int16_t basePupilRadius            {20};
    static constexpr int16_t pupilDistanceFromCenter    {15};               // distance from iris center - will be scaled after
    static constexpr int16_t basePupilSourceHeight      {25};               // higher = looks further down
    static constexpr uint16_t blinkInterval             {5 * 1000};
    static constexpr uint16_t irisPosQueueLen           {5};                // max eye movements + 1
    static constexpr uint16_t lookAroundPause           {2000};             // how long it looks in a specific direction

    // ===== Colors =====
    static constexpr uint16_t transparentColor          {0xABCD};
    static constexpr uint16_t backgroundColor           {TFT_WHITE};
    static constexpr uint16_t lidColor                  {TFT_BLACK};
    static constexpr uint16_t irisBaseColor             {TFT_BLACK};
    static constexpr uint16_t irisAngryColor            {TFT_RED};
    static constexpr uint16_t pupilColor                {TFT_WHITE};

    // ===== Derived Constants =====
    static constexpr int16_t halfWidth {static_cast<int16_t>(eyeWidth / 2)};
    static constexpr int16_t p2p3Offset {static_cast<int16_t>(0.28 * eyeWidth)};
    static constexpr float angularCoeffClosingSeq {-static_cast<float>(baseAperture) / blinkClosingTime};
    static constexpr float angularCoeffOpeningSeq {static_cast<float>(baseAperture) / blinkOpeningTime};
    static constexpr int16_t irisRLDistance {static_cast<int16_t>((irisRLDistanceCoeff * eyeWidth))};

    // ===== Runtime-calculated sprite dimensions =====
    int16_t spriteWidth {};
    int16_t spriteHeight {};
    int16_t baselineY {};                                                   // baseline position within sprite (changes based on actual bounds)
    int16_t spriteOffsetY {};                                               // offset to apply when pushing to screen

    // ===== Blink =====
    bool blinking {false};
    uint8_t currentAperture {baseAperture};
    uint32_t startBlink {};

    // ===== Iris =====
    int16_t irisPosXc {static_cast<int16_t>(halfWidth)};
    int16_t irisPosYc {};                                                   // set after calculating sprite bounds
    IrisPosition currentIrisPos {center};
    IrisPosition targetIrisPosition {center};
    int32_t startIrisMove {};
    uint16_t irisColor {irisBaseColor};
    int16_t irisPosQueueIndex {};
    IrisPosition irisPosQueue[irisPosQueueLen] {};
    uint32_t startIrisMoveWait {};
    uint32_t randIrisMoveWait {};
    uint32_t startLookAround {};

    // ===== Pupil =====
    int16_t pupilRadius {basePupilRadius};
    int16_t pupilX {};                                                      // wrt sumSprite
    int16_t pupilY {};                                                      // wrt sumSprite
    int16_t pupilSourceHeight {basePupilSourceHeight};

    void calculateSpriteBounds();
    void drawEyelids(uint8_t aperturePercentage);
    void updateBlink();
    void drawIris();
    void updateIrisPos();
    void updateIris();
    void generateIrisPositionQueue();
    void computePupilPosition();
    void randomStateUpdate();

public:
    void begin(DisplayManager* displayManager);
    void update();
    void setAlarmState(AlarmState as);
    void push() const;
    ~EyeSprite();
};



class KeypadKey {
private:

    static TFT_eSPI* tft;

    static constexpr uint8_t textSize {1};
    static constexpr uint8_t textFont {4};
    static int16_t width;
    static int16_t height;
    static int16_t cornerRadius;
    int16_t posX {};
    int16_t posY {};
    
    char ch {' '};

    static uint16_t bgColor;
    static uint16_t bgSelectColor;
    static uint16_t charColor;

    bool selected = {false};


public:
    KeypadKey() = default;
    KeypadKey(char ch, int16_t posX, int16_t posY);
    void setup(int16_t width, int16_t height, int16_t cornerRadius, uint16_t bgColor, uint16_t bgSelectColor, uint16_t charColor, TFT_eSPI* tft);
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

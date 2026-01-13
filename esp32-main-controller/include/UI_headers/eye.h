#pragma once
#include "displayManager.h"
#include "globals.h"

class EyeSprite {
private:

    // ===== Sprites =====
    DisplayManager* displayManager;
    TFT_eSprite* sumSprite;
    TFT_eSPI* tft;

    // ===== General Stuff =====
    
    uint32_t startTime {};
    enum IrisPosition {left, center, right,};
    bool initialized {false};
    bool needPush {true};

    // ===== Configuration Constants =====
    static constexpr int16_t posX                       {40};               // wrt screen
    static constexpr int16_t posY                       {100};              // wrt screen
    static constexpr int16_t eyeWidth                   {240};              // determines bezier curve width
    static constexpr float topPMaxHeight                {0.48f};            // control point for bezier curve generation
    static constexpr float bottomPMaxHeight             {0.41f};            // control point for bezier curve generation
    static constexpr float dozyMinuend                  {0.15f};             // minimum 0.05
    static constexpr uint8_t baseAperture               {90};
    static constexpr float tIncrease                    {0.003f};           // decrese to avoid zebra stripes on lids
    static constexpr uint16_t blinkClosingTime          {70};
    static constexpr uint16_t blinkOpeningTime          {130};
    static constexpr int16_t irisRadius                 {38};
    static constexpr float irisPosYcLoweringCoeff       {0.03f};            // makes iris lower ( * eyeWidth )
    static constexpr float irisRLDistanceCoeff          {0.22f};            // 0 ~ 0.5, how far to the side it can look
    static constexpr float irisVelocity                 {0.5f};
    static constexpr int16_t basePupilRadius            {20};
    static constexpr int16_t pupilDistanceFromCenter    {15};               // distance from iris center - will be scaled after
    static constexpr int16_t basePupilSourceHeight      {25};               // higher = looks further down
    static constexpr uint16_t blinkInterval             {5 * 1000};
    static constexpr uint16_t irisPosQueueLen           {5};                // max eye movements + 1
    static constexpr uint16_t lookAroundPause           {2000};             // how long it looks in a specific direction

    // ===== Colors =====
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
    // void setAlarmState(AlarmState as);
    void push(bool forcePush=false);
    ~EyeSprite();
};
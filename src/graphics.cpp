#include "graphics.h"

void DisplayManager::begin() {
    if (initialized) return;
    tft.init();
    tft.setRotation(0);     // pins at the top
    fillColor();
    initialized = true;
    Serial.println("Display initialized!");
}

void DisplayManager::fillColor(uint16_t color) {
    tft.fillScreen(color);
}

int16_t DisplayManager::width() {
    return tft.width();
}

int16_t DisplayManager::height() {
    return tft.height();
}

TFT_eSprite* DisplayManager::createSprite(int16_t w, int16_t h) {
    TFT_eSprite* sprite = new TFT_eSprite(&tft);
    sprite->createSprite(w, h);
    return sprite;
}

void DisplayManager::deleteSprite(TFT_eSprite* sprite) {
    if (sprite) {
        sprite->deleteSprite();
        delete sprite;
    }
}

TFT_eSPI* DisplayManager::getTFT() {
    return &tft;
}


void EyeSprite::calculateSpriteBounds() {
    // Calculate actual min/max Y by iterating through the bezier curves
    float minY = 0;
    float maxY = 0;
    
    float tempBaseline = 0;
    
    {
        float distFromBaseline = -topPMaxHeight * eyeWidth;
        int16_t x1 = halfWidth - p2p3Offset;
        int16_t y1 = tempBaseline + distFromBaseline;
        int16_t x2 = halfWidth + p2p3Offset;
        int16_t y2 = y1;
        
        for (float t = 0.0; t <= 1.0; t += tIncrease) {
            float yt = (1 - t) * (1 - t) * (1 - t) * tempBaseline + 
                       3 * (1 - t) * (1 - t) * t * y1 + 
                       3 * (1 - t) * t * t * y2 + 
                       t * t * t * tempBaseline;
            if (yt < minY) minY = yt;
        }
    }    {
        float distFromBaseline = bottomPMaxHeight * eyeWidth;
        int16_t x1 = halfWidth - p2p3Offset;
        int16_t y1 = tempBaseline + distFromBaseline;
        int16_t x2 = halfWidth + p2p3Offset;
        int16_t y2 = y1;
        
        for (float t = 0.0; t <= 1.0; t += tIncrease) {
            float yt = (1 - t) * (1 - t) * (1 - t) * tempBaseline + 
                       3 * (1 - t) * (1 - t) * t * y1 + 
                       3 * (1 - t) * t * t * y2 + 
                       t * t * t * tempBaseline;
            if (yt > maxY) maxY = yt;
        }
    }
    
    spriteWidth = eyeWidth;
    spriteHeight = static_cast<int16_t>(maxY - minY) + 1;
    baselineY = static_cast<int16_t>(-minY);
    spriteOffsetY = static_cast<int16_t>(minY);
    
    irisPosYc = baselineY;
}



void EyeSprite::drawEyelids(uint8_t aperturePercentage) {
    float topAperture;
    if (alarmState != disarmed) {
        topAperture = ((aperturePercentage * (topPMaxHeight - 0.05)) / 100);
    } else {
        topAperture = ((aperturePercentage * (topPMaxHeight - dozyMinuend)) / 100);
    }
    float bottomAperture {((aperturePercentage * bottomPMaxHeight) / 100)};
    
    const int maxPoints = static_cast<int>(1.0 / tIncrease) + 1;
    {
        float outerDist = -topPMaxHeight * eyeWidth;
        float innerDist = -topAperture * eyeWidth;
        
        int16_t x1_outer = halfWidth - p2p3Offset;
        int16_t y1_outer = baselineY + outerDist;
        int16_t x2_outer = halfWidth + p2p3Offset;
        int16_t y2_outer = y1_outer;
        
        int16_t x1_inner = halfWidth - p2p3Offset;
        int16_t y1_inner = baselineY + innerDist;
        int16_t x2_inner = halfWidth + p2p3Offset;
        int16_t y2_inner = y1_inner;
        
        float prev_xt_outer = 0;
        float prev_yt_outer = baselineY;
        float prev_xt_inner = 0;
        float prev_yt_inner = baselineY;
        
        for (float t = 0.0; t <= 1.0; t += tIncrease) {
            // Outer curve point
            float xt_outer = (1 - t) * (1 - t) * (1 - t) * 0 + 3 * (1 - t) * (1 - t) * t * x1_outer + 
                            3 * (1 - t) * t * t * x2_outer + t * t * t * spriteWidth;
            float yt_outer = (1 - t) * (1 - t) * (1 - t) * baselineY + 3 * (1 - t) * (1 - t) * t * y1_outer + 
                            3 * (1 - t) * t * t * y2_outer + t * t * t * baselineY;
            
            // Inner curve point
            float xt_inner = (1 - t) * (1 - t) * (1 - t) * 0 + 3 * (1 - t) * (1 - t) * t * x1_inner + 
                            3 * (1 - t) * t * t * x2_inner + t * t * t * spriteWidth;
            float yt_inner = (1 - t) * (1 - t) * (1 - t) * baselineY + 3 * (1 - t) * (1 - t) * t * y1_inner + 
                            3 * (1 - t) * t * t * y2_inner + t * t * t * baselineY;
            
            sumSprite->drawLine(static_cast<int16_t>(xt_outer), static_cast<int16_t>(yt_outer), 
                              static_cast<int16_t>(xt_inner), static_cast<int16_t>(yt_inner), lidColor);
            
            prev_xt_outer = xt_outer;
            prev_yt_outer = yt_outer;
            prev_xt_inner = xt_inner;
            prev_yt_inner = yt_inner;
        }
    }
    
    {
        float innerDist = bottomAperture * eyeWidth;
        float outerDist = bottomPMaxHeight * eyeWidth;
        
        int16_t x1_inner = halfWidth - p2p3Offset;
        int16_t y1_inner = baselineY + innerDist;
        int16_t x2_inner = halfWidth + p2p3Offset;
        int16_t y2_inner = y1_inner;
        
        int16_t x1_outer = halfWidth - p2p3Offset;
        int16_t y1_outer = baselineY + outerDist;
        int16_t x2_outer = halfWidth + p2p3Offset;
        int16_t y2_outer = y1_outer;
        
        for (float t = 0.0; t <= 1.0; t += tIncrease) {
            // Inner curve point
            float xt_inner = (1 - t) * (1 - t) * (1 - t) * 0 + 3 * (1 - t) * (1 - t) * t * x1_inner + 
                            3 * (1 - t) * t * t * x2_inner + t * t * t * spriteWidth;
            float yt_inner = (1 - t) * (1 - t) * (1 - t) * baselineY + 3 * (1 - t) * (1 - t) * t * y1_inner + 
                            3 * (1 - t) * t * t * y2_inner + t * t * t * baselineY;
            
            // Outer curve point
            float xt_outer = (1 - t) * (1 - t) * (1 - t) * 0 + 3 * (1 - t) * (1 - t) * t * x1_outer + 
                            3 * (1 - t) * t * t * x2_outer + t * t * t * spriteWidth;
            float yt_outer = (1 - t) * (1 - t) * (1 - t) * baselineY + 3 * (1 - t) * (1 - t) * t * y1_outer + 
                            3 * (1 - t) * t * t * y2_outer + t * t * t * baselineY;
            
            sumSprite->drawLine(static_cast<int16_t>(xt_inner), static_cast<int16_t>(yt_inner), 
                              static_cast<int16_t>(xt_outer), static_cast<int16_t>(yt_outer), lidColor);
        }
    }
}

void EyeSprite::updateBlink() {
    if (!startBlink) {
        startBlink = millis();
    }
    uint32_t elapsed {millis() - startBlink};

    if (elapsed <= blinkClosingTime) {
        currentAperture = floor(angularCoeffClosingSeq * elapsed + baseAperture);
    } else if (elapsed <= (blinkClosingTime + blinkOpeningTime)) {
        currentAperture = floor(angularCoeffOpeningSeq * (elapsed - blinkClosingTime));
    } else {
        startBlink = 0;
        blinking = false;
        currentAperture = baseAperture;
    }
}

void EyeSprite::drawIris() {
    sumSprite->fillCircle(irisPosXc, irisPosYc, irisRadius, irisColor);
    sumSprite->fillCircle(pupilX, pupilY, pupilRadius, pupilColor);
}

void EyeSprite::updateIrisPos() {
    if (!startIrisMove) startIrisMove = millis();

    int8_t coeffMovementDirection {1};
    if (currentIrisPos < targetIrisPosition) {
        coeffMovementDirection = 1;
    } else {
        coeffMovementDirection = -1;
    }

    int16_t s0 {};
    if (currentIrisPos == center) s0 = halfWidth;
    else if (currentIrisPos == left) s0 = halfWidth - irisRLDistance;
    else if (currentIrisPos == right) s0 = halfWidth + irisRLDistance;

    uint32_t elapsed = millis() - startIrisMove;
    irisPosXc = s0 + static_cast<uint16_t>(coeffMovementDirection * irisVelocity * elapsed);

    if (targetIrisPosition == center && 
        ((coeffMovementDirection == 1 && irisPosXc >= halfWidth) ||
            (coeffMovementDirection == -1 && irisPosXc <= halfWidth))) {
        irisPosXc = halfWidth;
        startIrisMove = 0;
        currentIrisPos = targetIrisPosition;
    } else if (targetIrisPosition == left && irisPosXc <= halfWidth - irisRLDistance) {
        irisPosXc = halfWidth - irisRLDistance;
        startIrisMove = 0;
        currentIrisPos = targetIrisPosition;
    } else if (targetIrisPosition == right && irisPosXc >= halfWidth + irisRLDistance) {
        irisPosXc = halfWidth + irisRLDistance;
        startIrisMove = 0;
        currentIrisPos = targetIrisPosition;
    }
}

void EyeSprite::updateIris() {
    if (currentIrisPos != targetIrisPosition) {
        updateIrisPos();
        computePupilPosition();
    }
}

void EyeSprite::generateIrisPositionQueue() {
    int16_t arrayLen {sizeof(irisPosQueue) / sizeof(irisPosQueue[0])};
    for (int16_t i {0}; i < arrayLen; i++) {
        irisPosQueue[i] = center;
    }
    int8_t numberOfEyeMovements(random(1, arrayLen));
    IrisPosition lastPos {random(2) ? right : left};
    for (int16_t i {0}; i < numberOfEyeMovements; i++) {
        if (lastPos == right) irisPosQueue[i] = left;
        else irisPosQueue[i] = right;
        lastPos = lastPos==left ? right : left;
    }
    irisPosQueueIndex = 0;
}

void EyeSprite::computePupilPosition() {
    float dx = irisPosXc - (halfWidth);
    float dy = irisPosYc - baselineY + pupilSourceHeight;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < 0.1) {
        pupilX = irisPosXc;
        pupilY = irisPosYc;
        return;
    }
    float maxIrisDistance = irisRLDistance;
    float distanceRatio = distance / maxIrisDistance;
    float scaledPupilDistance = pupilDistanceFromCenter * distanceRatio;

    float scale = scaledPupilDistance / distance;
    float pupilCenterX = dx * scale;
    float pupilCenterY = dy * scale;

    pupilX = irisPosXc + static_cast<int16_t>(pupilCenterX);
    pupilY = irisPosYc + static_cast<int16_t>(pupilCenterY);
}

void EyeSprite::randomStateUpdate() {
    uint32_t elapsed {millis() - startTime};
    if ((elapsed % blinkInterval) < blinkClosingTime) {
        blinking = true;
    }
    if ((alarmState==soundAlarm || alarmState==lockdown) && currentIrisPos==center) return;

    if (!randIrisMoveWait) {
        startIrisMoveWait = elapsed;

        if (alarmState==disarmed) randIrisMoveWait = random((6*1000), (15*1000));
        else randIrisMoveWait = random((4*1000), (8*1000));
        generateIrisPositionQueue();
    }
    if (startIrisMoveWait + randIrisMoveWait < elapsed) {
        if (!startLookAround) {
            startLookAround = elapsed;
        }

        if (elapsed > startLookAround + (lookAroundPause * irisPosQueueIndex)) {
            IrisPosition nextPos {irisPosQueue[irisPosQueueIndex]};
            targetIrisPosition = nextPos;
            if (nextPos == center) {
                irisPosQueueIndex = 0;
                startLookAround = 0;
                randIrisMoveWait = 0;
            }
            else {
                irisPosQueueIndex++;
            }
        }
    }
}

void EyeSprite::begin(DisplayManager* dM) {
    if (initialized) return;
    initialized = true;

    startTime = millis();
    displayManager = dM;
    
    calculateSpriteBounds();
    sumSprite = displayManager->createSprite(spriteWidth, spriteHeight);

    computePupilPosition();
}

void EyeSprite::update() {
    sumSprite->fillScreen(backgroundColor);

    randomStateUpdate();

    if (blinking) updateBlink();
    updateIris();

    drawIris();
    drawEyelids(currentAperture);
}

void EyeSprite::setAlarmState(AlarmState as) {
    alarmState = as;
    switch (as)
    {
    case disarmed:
        irisPosYc = static_cast<int16_t>(baselineY + irisPosYcLoweringCoeff * eyeWidth);
        irisColor = irisBaseColor;
        pupilSourceHeight = basePupilSourceHeight;
        pupilRadius = basePupilRadius;
        break;
    case armedHome:
    case armedAway:
        irisPosYc = baselineY;
        irisColor = irisBaseColor;
        pupilSourceHeight = 0;
        pupilRadius = basePupilRadius;
        break;
    case soundAlarm:
    case lockdown:
        irisPosYc = baselineY;
        irisColor = irisAngryColor;
        pupilSourceHeight = 0;
        pupilRadius = basePupilRadius - 4;
        break;
    default:
        break;
    }
    updateIrisPos();
    computePupilPosition();
}

void EyeSprite::push() const {
    // Push directly to screen - no need to composite sprites
    sumSprite->pushSprite(posX, posY + spriteOffsetY, transparentColor);
}

EyeSprite::~EyeSprite() {
    if (displayManager != nullptr) {
        displayManager->deleteSprite(sumSprite);
    }
}







TFT_eSPI* KeypadKey::tft {nullptr};
int16_t KeypadKey::width {};
int16_t KeypadKey::height {};
int16_t KeypadKey::cornerRadius {};
uint16_t KeypadKey::bgColor {};
uint16_t KeypadKey::bgSelectColor {};
uint16_t KeypadKey::charColor {};

KeypadKey::KeypadKey(char ch, int16_t posX, int16_t posY) : ch{ch}, posX{posX}, posY{posY} {}

void KeypadKey::setup(int16_t w, int16_t h, int16_t cR, uint16_t bg, uint16_t bgSel, uint16_t chr, TFT_eSPI* tft) {
    KeypadKey::width = w;
    KeypadKey::height = h;
    KeypadKey::cornerRadius = cR;

    KeypadKey::bgColor = bg;
    KeypadKey::bgSelectColor = bgSel;
    KeypadKey::charColor = chr;

    KeypadKey::tft = tft;
}

void KeypadKey::push() {
    tft->fillRoundRect(posX, posY, width, height, cornerRadius, selected ? bgSelectColor : bgColor);

    int16_t textX {static_cast<int16_t>(posX + width / 2 - 8)};   // approx centering for size 2
    int16_t textY {static_cast<int16_t>(posY + height / 2 - 8)};  // approx centering for size 2
    tft->setTextColor(charColor);

    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    tft->setCursor(textX, textY);
    tft->print(ch);
}

void KeypadKey::select(bool s) {
    selected = s;
}






void KeypadGraph::begin(DisplayManager* dM) {
    if (initialized) return;

    if (!dM) return;
    startTime = millis();
    displayManager = dM;
    tft = displayManager->getTFT();

    int16_t keyHeight {static_cast<int16_t>(((height - pinProgressHeight) / 4) - 2 * keySidePaddingY)};
    int16_t keyWidth {static_cast<int16_t>((width / 3) - 2 * keySidePaddingX)};

    for (int i {}; i < 12; i++) {
        int16_t keyPosX = static_cast<int16_t>(posX + keySidePaddingX + (i % 3) * (keyWidth + 2 * keySidePaddingX));
        int16_t keyPosY = static_cast<int16_t>(posY + pinProgressHeight + keySidePaddingY + (i / 3) * (keyHeight + 2 * keySidePaddingY));

        keyArray[i] = KeypadKey{allChars[i], keyPosX, keyPosY};
    }
    keyArray[0].setup(keyWidth, keyHeight, cornerRadius, keyBgColor, keyBgSelectColor, charColor, tft);

    initialized = true;
}

void KeypadGraph::selectKey(char ch) {
    for (int i {}; allChars[i] != '\0'; i++) {
        if (allChars[i] == ch) {
            keyArray[i].select(true);
            keyArray[i].push();
            break;
        }
    }
}

void KeypadGraph::deselectKey(char ch) {
    for (int i {}; allChars[i] != '\0'; i++) {
        if (allChars[i] == ch) {
            keyArray[i].select(false);
            keyArray[i].push();
            break;
        }
    }
}

void KeypadGraph::pushAll() {
    tft->fillRect(posX, posY, width, height, bgColor);
    for (int i {}; allChars[i] != '\0'; i++) {
        keyArray[i].push();
    }
    for (int i {}; i < 6; i++) {
        pushBufferedNumber(i, false);
    }
}

void KeypadGraph::pushBufferedNumber(int16_t position, bool selected) {

    int16_t posX {static_cast<int16_t>((KeypadGraph::posX + ((width - pinProgressWidth) / 2)) + position * (pinProgressWidth / 5))};
    static int16_t circleCenterY {KeypadGraph::posY + pinProgressHeight / 2};
    static int16_t rectTopLeftY {static_cast<int16_t>(circleCenterY - (pinProgressRectHeight / 2))};

    if (selected) {
        tft->fillCircle(posX, circleCenterY, pinProgressCircleRadius, charColor);
    }
    else {
        tft->fillCircle(posX, circleCenterY, pinProgressCircleRadius, bgColor);
        tft->fillRoundRect(posX - pinProgressCircleRadius, rectTopLeftY, pinProgressCircleRadius * 2, 
            pinProgressRectHeight, pinProgressRectHeight / 2, charColor);
    }
}

void KeypadGraph::setBufferedNumbers(int16_t numberOfBufferedNumbers) {
    if (numberOfBufferedNumbers > 6) return;
    if (previousSelectedNumbers == numberOfBufferedNumbers) return;
    else if (previousSelectedNumbers > numberOfBufferedNumbers) {
        for (int i {}; i < (previousSelectedNumbers - numberOfBufferedNumbers); i++) {
            pushBufferedNumber(numberOfBufferedNumbers + i, false);
        }
    }
    else /* previousSelectedNumbers < numberOfBufferedNumbers */{
        for (int i {}; i < (numberOfBufferedNumbers - previousSelectedNumbers); i++) {
            pushBufferedNumber(previousSelectedNumbers + i, true);
        }
    }
    previousSelectedNumbers = numberOfBufferedNumbers;
}


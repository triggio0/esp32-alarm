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



void EyeSprite::drawEyelid(float distFromBaseline, uint16_t color, bool outlineMode) {
    int16_t x0 {static_cast<int16_t>((outlineMode ? 0 : 1))};
    int16_t y0 {midH};

    int16_t x1 {static_cast<int16_t>((halfWidth - p2p3Offset))};
    int16_t y1 {static_cast<int16_t>((midH + distFromBaseline))};

    int16_t x2 {static_cast<int16_t>((halfWidth + p2p3Offset))};
    int16_t y2 {y1};

    int16_t x3 {static_cast<int16_t>((outlineMode ? width : width -1))};
    int16_t y3 {midH};

    float prev_xt = x0;
    float prev_yt = y0;

    for (float t {0.0}; t <= 1; t += tIncrease) {
        float xt = (1 - t) * (1 - t) * (1 - t) * x0 + 3 * (1 - t) * (1 - t) * t * x1 + 3 * (1 - t) * t * t * x2 + t * t * t * x3;
        float yt = (1 - t) * (1 - t) * (1 - t) * y0 + 3 * (1 - t) * (1 - t) * t * y1 + 3 * (1 - t) * t * t * y2 + t * t * t * y3;
        if (!outlineMode) {
            lidsSprite->drawLine(xt, yt, xt, midH, color);
        } else {
            lidsSprite->drawLine(static_cast<int16_t>(prev_xt), static_cast<int16_t>(prev_yt), static_cast<int16_t>(xt), static_cast<int16_t>(yt), color);
            prev_xt = xt;
            prev_yt = yt;
        }
    }
}

void EyeSprite::drawEyelids(uint8_t aperturePercentage) {
    float topAperture;
    if (alarmState != disarmed) {
        topAperture = ((aperturePercentage * (topPMaxHeight - 0.05)) / 100);
    } else {
        topAperture = ((aperturePercentage * (topPMaxHeight - dozyMinuend)) / 100);
    }
    float bottomAperture {((aperturePercentage * bottomPMaxHeight) / 100)};
    drawEyelid(-topPMaxHeight * width, lidColor);               // top outer
    drawEyelid(bottomPMaxHeight * width, lidColor);             // bottom outer
    drawEyelid(-topAperture * width, transparentColor);         // top inner
    drawEyelid(bottomAperture * width, transparentColor);       // bottom inner
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
    irisSprite->fillCircle(irisRadius, irisRadius, irisRadius, irisColor);
    irisSprite->fillCircle(pupilX, pupilY, pupilRadius, pupilColor);
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
    float dy = irisPosYc - midH + pupilSourceHeight;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < 0.1) {       // to avoid 0 division
        pupilX = irisRadius;
        pupilY = irisRadius;
        return;
    }
    float maxIrisDistance = irisRLDistance;  // Maximum distance iris moves from center
    float distanceRatio = distance / maxIrisDistance;  // 0.0 (centered) to 1.0 (max)
    float scaledPupilDistance = pupilDistanceFromCenter * distanceRatio;

    float scale = scaledPupilDistance / distance;
    float pupilCenterX = dx * scale;
    float pupilCenterY = dy * scale;

    pupilX = irisRadius + static_cast<int16_t>(pupilCenterX);
    pupilY = irisRadius + static_cast<int16_t>(pupilCenterY);
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
    sumSprite = displayManager->createSprite(width, height);
    lidsSprite = displayManager->createSprite(width, height);
    irisSprite = displayManager->createSprite(irisRadius * 2 + 1, irisRadius * 2 + 1);

    drawEyelid(-topPMaxHeight * width, lidColor, true);     // top outer
    drawEyelid(bottomPMaxHeight * width, lidColor, true);   // bottom outer
    computePupilPosition();
}

void EyeSprite::update() {
    sumSprite->fillScreen(backgroundColor);
    lidsSprite->fillSprite(backgroundColor);
    irisSprite->fillSprite(transparentColor);

    randomStateUpdate();

    if (blinking) updateBlink();
    updateIris();

    drawEyelids(currentAperture);
    drawIris();
}

void EyeSprite::setAlarmState(AlarmState as) {
    alarmState = as;
    switch (as)
    {
    case disarmed:
        irisPosYc = static_cast<int16_t>(midH + irisPosYcLoweringCoeff * width);
        irisColor = irisBaseColor;
        pupilSourceHeight = basePupilSourceHeight;
        pupilRadius = basePupilRadius;
        break;
    case armedHome:
    case armedAway:
        irisPosYc = midH;
        irisColor = irisBaseColor;
        pupilSourceHeight = 0;
        pupilRadius = basePupilRadius;
        break;
    case soundAlarm:
    case lockdown:
        irisPosYc = midH;
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
    irisSprite->pushToSprite(sumSprite, (irisPosXc - irisRadius), (irisPosYc - irisRadius), transparentColor);
    lidsSprite->pushToSprite(sumSprite, 0, 0, transparentColor);
    sumSprite->pushSprite(posX, posY, transparentColor);
}

EyeSprite::~EyeSprite() {
    if (displayManager != nullptr) {
        displayManager->deleteSprite(sumSprite);
        displayManager->deleteSprite(lidsSprite);
        displayManager->deleteSprite(irisSprite);
    }
}




KeypadKey::KeypadKey(char ch, int16_t posX, int16_t posY, int16_t width, int16_t height, uint16_t bgColor, uint16_t bgSelectColor, uint16_t charColor) : 
    ch(ch), posX(posX), posY(posY), width(width), height(height), bgColor(bgColor), bgSelectColor(bgSelectColor), charColor(charColor) {
}



void KeypadSprite::begin(DisplayManager* dM) {
    if (initialized) return;
    initialized = true;

    startTime = millis();

    sumSprite = displayManager->createSprite(width, height);

    int16_t keyHeight {height / 4};
    int16_t keyWidth {width / 3};

    for (int i {}; i < 12; i++) {
        int16_t keyPosX {posX + (i % 3) * keyWidth};
        int16_t keyPosY {posY + pinProgressHeight + (1 / 3) * keyHeight};

        keyArray[i] = KeypadKey{allChars[i], keyPosX, keyPosY, keyWidth, keyHeight, keyBgColor, keyBgSelectColor, charColor, };
    }
}

KeypadSprite::~KeypadSprite() {
    displayManager->deleteSprite(sumSprite);
}
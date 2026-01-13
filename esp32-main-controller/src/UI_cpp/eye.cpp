#include "Ui_headers/eye.h"
#include "Ui_headers/displayManager.h"

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
    
    irisPosYc = static_cast<int16_t>(baselineY + irisPosYcLoweringCoeff * eyeWidth);
}

void EyeSprite::drawEyelids(uint8_t aperturePercentage) {
    float topAperture;
    if (System::alarmState != disarmed) {
        topAperture = ((aperturePercentage * (topPMaxHeight - 0.05)) / 100);
    } else {
        topAperture = ((aperturePercentage * (topPMaxHeight - dozyMinuend)) / 100);
    }
    float bottomAperture = ((aperturePercentage * bottomPMaxHeight) / 100);
    
    auto drawEyelidCurve = [this](float innerDist, float outerDist) {
        int16_t x1_outer = halfWidth - p2p3Offset;
        int16_t y1_outer = baselineY + outerDist;
        int16_t x2_outer = halfWidth + p2p3Offset;
        
        int16_t x1_inner = halfWidth - p2p3Offset;
        int16_t y1_inner = baselineY + innerDist;
        int16_t x2_inner = halfWidth + p2p3Offset;
        
        for (float t = 0.0; t <= 1.0; t += tIncrease) {
            float oneMinusT = 1 - t;
            float oneMinusTSquared = oneMinusT * oneMinusT;
            float oneMinusTCubed = oneMinusTSquared * oneMinusT;
            float tSquared = t * t;
            float tCubed = tSquared * t;
            
            // Outer curve point
            float xt_outer = oneMinusTCubed * 0 + 3 * oneMinusTSquared * t * x1_outer + 
                           3 * oneMinusT * tSquared * x2_outer + tCubed * spriteWidth;
            float yt_outer = oneMinusTCubed * baselineY + 3 * oneMinusTSquared * t * y1_outer + 
                           3 * oneMinusT * tSquared * y1_outer + tCubed * baselineY;
            
            // Inner curve point
            float xt_inner = oneMinusTCubed * 0 + 3 * oneMinusTSquared * t * x1_inner + 
                           3 * oneMinusT * tSquared * x2_inner + tCubed * spriteWidth;
            float yt_inner = oneMinusTCubed * baselineY + 3 * oneMinusTSquared * t * y1_inner + 
                           3 * oneMinusT * tSquared * y1_inner + tCubed * baselineY;
            
            sumSprite->drawLine(static_cast<int16_t>(xt_outer), static_cast<int16_t>(yt_outer), 
                              static_cast<int16_t>(xt_inner), static_cast<int16_t>(yt_inner), lidColor);
        }
    };
    
    drawEyelidCurve(-topAperture * eyeWidth, -topPMaxHeight * eyeWidth);
    drawEyelidCurve(bottomAperture * eyeWidth, bottomPMaxHeight * eyeWidth);
}

void EyeSprite::updateBlink() {
    needPush = true;
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
        needPush = true;
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
    if ((System::alarmState==soundAlarm || System::alarmState==lockdown) && currentIrisPos==center) return;

    if (!randIrisMoveWait) {
        startIrisMoveWait = elapsed;

        if (System::alarmState==disarmed) randIrisMoveWait = random((6*1000), (15*1000));
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
    tft = displayManager->getTFT();

    calculateSpriteBounds();
    sumSprite = displayManager->createSprite(spriteWidth, spriteHeight);

    computePupilPosition();
    update();
}

void EyeSprite::update() {
    sumSprite->fillScreen(backgroundColor);

    randomStateUpdate();

    if (blinking) updateBlink();
    updateIris();

    drawIris();
    drawEyelids(currentAperture);
}

// void EyeSprite::setAlarmState(AlarmState as) {
//     alarmState = as;
//     switch (as)
//     {
//     case disarmed:
//         irisPosYc = static_cast<int16_t>(baselineY + irisPosYcLoweringCoeff * eyeWidth);
//         irisColor = irisBaseColor;
//         pupilSourceHeight = basePupilSourceHeight;
//         pupilRadius = basePupilRadius;
//         break;
//     case armedHome:
//     case armedAway:
//         irisPosYc = baselineY;
//         irisColor = irisBaseColor;
//         pupilSourceHeight = 0;
//         pupilRadius = basePupilRadius;
//         break;
//     case soundAlarm:
//     case lockdown:
//         irisPosYc = baselineY;
//         irisColor = irisAngryColor;
//         pupilSourceHeight = 0;
//         pupilRadius = basePupilRadius - 6;
//         break;
//     default:
//         break;
//     }
//     updateIrisPos();
//     computePupilPosition();
// }

void EyeSprite::push(bool forcePush) {
    // sumSprite->pushSprite(posX, posY + spriteOffsetY);

    if (!needPush && !forcePush) return;
    needPush = false;

    tft->startWrite();
    tft->setAddrWindow(posX, posY + spriteOffsetY, spriteWidth, spriteHeight);
    uint16_t* buffer = (uint16_t*)sumSprite->getPointer();
    tft->pushPixels(buffer, spriteWidth * spriteHeight);
    tft->endWrite();
}

EyeSprite::~EyeSprite() {
    if (displayManager != nullptr) {
        displayManager->deleteSprite(sumSprite);
    }
}

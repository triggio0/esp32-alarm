#include "Ui_headers/keypad.h"

TFT_eSPI* KeypadKey::tft {nullptr};
int16_t KeypadKey::width {};
int16_t KeypadKey::height {};
int16_t KeypadKey::cornerRadius {};
uint16_t KeypadKey::bgColor {};
uint16_t KeypadKey::bgSelectColor {};
uint16_t KeypadKey::charColor {};

KeypadKey::KeypadKey(char ch, int16_t posX, int16_t posY) : ch{ch}, posX{posX}, posY{posY} {}

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


void KeypadGraph::begin(DisplayManager* dM, TouchScreenManager* tsM) {
    if (!dM || !tsM) return;
    startTime = millis();
    displayManager = dM;
    tsManager = tsM;
    tft = displayManager->getTFT();

    int16_t keyHeight {static_cast<int16_t>(((height - pinProgressHeight) / 4) - 2 * keySidePaddingY)};
    int16_t keyWidth {static_cast<int16_t>((width / 3) - 2 * keySidePaddingX)};

    for (int i {}; i < 12; i++) {
        int16_t keyPosX = static_cast<int16_t>(posX + keySidePaddingX + (i % 3) * (keyWidth + 2 * keySidePaddingX));
        int16_t keyPosY = static_cast<int16_t>(posY + pinProgressHeight + keySidePaddingY + (i / 3) * (keyHeight + 2 * keySidePaddingY));

        keyArray[i] = KeypadKey(allChars[i], keyPosX, keyPosY);

        TouchRect touchRect;
        touchRect.startX = keyPosX - keySidePaddingX;
        touchRect.startY = keyPosY - keySidePaddingY;
        touchRect.endX = keyPosX + keyWidth + keySidePaddingX;
        touchRect.endY = keyPosY + keyHeight + keySidePaddingY;
        std::function<void()> selectionCb;

        if (allChars[i] == 'C') {
            selectionCb = [this, i]() {
                if (!bufferedNumbers) {
                    setUnlockSequenceState(codeAbort);
                } else {
                    for (int j {}; j < 6; j++) pwBuffer[j] = '\0';
                    bufferedNumbers = 0;
                    pushBufferedNumbersDisplay();
                }
            };
        } else if (allChars[i] == '<') {
            selectionCb = [this, i]() {
                if (!bufferedNumbers) {
                    setUnlockSequenceState(codeAbort);
                } else {
                    for (int j {}; j < 6; j++) {
                        if (pwBuffer[j] == '\0') {
                            pwBuffer[j-1] = '\0';
                            break;
                        }
                    }
                    bufferedNumbers--;
                    pushBufferedNumbersDisplay();
                }
            };
        } else {
            selectionCb = [this, i]() {
                for (int j {}; j < 6; j++) {
                    if (pwBuffer[j] == '\0') {
                        pwBuffer[j] = keyArray[i].getChar();
                        break;
                    }
                }
                bufferedNumbers++;
                pushBufferedNumbersDisplay();
                if (bufferedNumbers == 6) {
                    checkCodeBuffer();
                }

            };
        }
        touchButtonArray[i] = TouchButton(
            touchRect,
            [this, i](bool selected) {
                keyArray[i].select(selected);
                keyArray[i].push();
            },
            selectionCb
        );
    }
    KeypadKey::width = keyWidth;
    KeypadKey::height = keyHeight;
    KeypadKey::cornerRadius = cornerRadius;
    KeypadKey::bgColor = keyBgColor;
    KeypadKey::bgSelectColor = keyBgSelectColor;
    KeypadKey::charColor = charColor;
    KeypadKey::tft = tft;
}

void KeypadGraph::checkCodeBuffer() {
    for (int i {}; i < 6; i++) {
        if (pwBuffer[i] != alarmPassword[i]) {
            bufferedNumbers = 0;
            for (int j {}; j < 6; j++) {
                pwBuffer[j] = '\0';
            }
            pushIncorrectNumbersDisplay();
            unlockSequenceState = codeFail;
            Serial.println("wrong code!");
            return;
        }
    }
    bufferedNumbers = 0;
    for (int j {}; j < 6; j++) {
        pwBuffer[j] = '\0';
    }
    unlockSequenceState = codeSuccess;
    Serial.println("correct code!");
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

void KeypadGraph::update() {
    if (unlockSequenceState == none) return;
    TouchPoint touchPoint {tsManager->getTouch()};

    static uint8_t consecInvalid;
    if (!touchPoint.valid) {
        if (++consecInvalid > 1) return;
    } else {
        consecInvalid = 0;
    }
    for (int i {}; i < 12; i++) {
        touchButtonArray[i].checkCollision(touchPoint);
    }
}

void KeypadGraph::pushBufferedNumber(int16_t position, bool selected, bool incorrect) {

    int16_t posX {static_cast<int16_t>((KeypadGraph::posX + ((width - pinProgressWidth) / 2)) + position * (pinProgressWidth / 5))};
    static int16_t circleCenterY {KeypadGraph::posY + pinProgressHeight / 2};
    static int16_t rectTopLeftY {static_cast<int16_t>(circleCenterY - (pinProgressRectHeight / 2))};

    if (selected) {
        tft->fillCircle(posX, circleCenterY, pinProgressCircleRadius, incorrect ? charColorIncorrect : charColor);
    }
    else {
        tft->fillCircle(posX, circleCenterY, pinProgressCircleRadius, bgColor);
        tft->fillRoundRect(posX - pinProgressCircleRadius, rectTopLeftY, pinProgressCircleRadius * 2, 
            pinProgressRectHeight, pinProgressRectHeight / 2, charColor);
    }
}

void KeypadGraph::pushBufferedNumbersDisplay() {
    Serial.printf("previous: %i | current: %i\n", previousBufferedNumbers, bufferedNumbers);
    if (bufferedNumbers > 6) return;
    if (previousBufferedNumbers == bufferedNumbers) return;
    else if (previousBufferedNumbers == 6 && bufferedNumbers == 1) {    // case where wrong code was inserted
        pushBufferedNumber(0, true);
        for (int i {1}; i < 6; i++) {
            pushBufferedNumber(i, false);
        } 
    }
    else if (previousBufferedNumbers > bufferedNumbers) {
        for (int i {}; i < (previousBufferedNumbers - bufferedNumbers); i++) {
            pushBufferedNumber(bufferedNumbers + i, false);
        }
    }
    else /* previousBufferedNumbers < bufferedNumbers */{
        for (int i {}; i < (bufferedNumbers - previousBufferedNumbers); i++) {
            pushBufferedNumber(previousBufferedNumbers + i, true);
        }
    }
    previousBufferedNumbers = bufferedNumbers;
}

void KeypadGraph::pushIncorrectNumbersDisplay() {
    for(int i {}; i < 6; i++) {
        pushBufferedNumber(i, true, true);
    }
}


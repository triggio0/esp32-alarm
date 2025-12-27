#include "TFT_h/keypad.h"
#include "TFT_h/displayManager.h"

TFT_eSPI* KeypadKey::tft {nullptr};
int16_t KeypadKey::width {};
int16_t KeypadKey::height {};
int16_t KeypadKey::cornerRadius {};
uint16_t KeypadKey::bgColor {};
uint16_t KeypadKey::bgSelectColor {};
uint16_t KeypadKey::charColor {};

KeypadKey::KeypadKey(char ch, int16_t posX, int16_t posY) : ch{ch}, posX{posX}, posY{posY} {}

// void KeypadKey::setup(int16_t w, int16_t h, int16_t cR, uint16_t bg, uint16_t bgSel, uint16_t chr, TFT_eSPI* tft) {
//     KeypadKey::width = w;
//     KeypadKey::height = h;
//     KeypadKey::cornerRadius = cR;

//     KeypadKey::bgColor = bg;
//     KeypadKey::bgSelectColor = bgSel;
//     KeypadKey::charColor = chr;

//     KeypadKey::tft = tft;
// }

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
    if (initialized) return;

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

        KeypadKey keypadKey {allChars[i], keyPosX, keyPosY};
        keyArray[i] = keypadKey;

        TouchRect touchRect;
        touchRect.startX = keyPosX - keySidePaddingX;
        touchRect.startY = keyPosY - keySidePaddingY;
        touchRect.endX = keyPosX + keyWidth + keySidePaddingX;
        touchRect.endY = keyPosY + keyHeight + keySidePaddingY;

        touchButtonArray[i] = TouchButton(
            touchRect,
            [&keypadKey](bool selected) {
                keypadKey.select(selected);
                keypadKey.push();
            },
            [&keypadKey]() {
                // TODO: button pressed callback (generic number)
            }
        );
    }
    KeypadKey::width = keyWidth;
    KeypadKey::height = keyHeight;
    KeypadKey::cornerRadius = cornerRadius;
    KeypadKey::bgColor = keyBgColor;
    KeypadKey::bgSelectColor = keyBgSelectColor;
    KeypadKey::charColor = charColor;
    KeypadKey::tft = tft;

    // TODO: add callback for < and C

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

void KeypadGraph::update() {
    TouchPoint touchPoint {tsManager->getTouch()};
    if (!touchPoint.valid) return;

    for (int i {}; i < 12; i++) {
        touchButtonArray[i].checkCollision(touchPoint);
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

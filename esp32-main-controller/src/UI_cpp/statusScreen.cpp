#include "Ui_headers/statusScreen.h"
#include "Ui_headers/displayManager.h"

// const std::unordered_map<StatusScreenEntries, const char*> entryNames {
//     {front_door, "Front door:"},
//     {window, "Window:"},
//     {window_sensor, "window sensor:"},
//     {camera, "Camera:"},
//     {wifi, "Wi-Fi:"},
// };

TFT_eSPI* StatusEntry::tft;
int16_t StatusEntry::firstLinePosX;
int16_t StatusEntry::firstLinePosY;
int16_t StatusEntry::lineHeight;
int16_t StatusEntry::lineWidth;
uint16_t StatusEntry::textColor;
uint16_t StatusEntry::bgColor;
uint16_t StatusEntry::trueColor;
uint16_t StatusEntry::falseColor;
int16_t StatusEntry::textSize;
int16_t StatusEntry::textFont;
int16_t StatusEntry::textIndentation;

StatusEntry::StatusEntry(StatusScreenEntries id, int16_t lineNumber, const char* entryName, const char* valueTrue, const char* valueFalse) 
    : entryID{id}, line{lineNumber}, value{false} {

    strncpy(displayedEntry, entryName, sizeof(displayedEntry) - 1);
    displayedEntry[sizeof(displayedEntry) - 1] = '\0';
    strncpy(displayedValueTrue, valueTrue, sizeof(displayedValueTrue) - 1);
    displayedValueTrue[sizeof(displayedValueTrue) - 1] = '\0';
    strncpy(displayedValueFalse, valueFalse, sizeof(displayedValueFalse) - 1);
    displayedValueFalse[sizeof(displayedValueFalse) - 1] = '\0';
}

void StatusEntry::pushRightSide() {

    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    if (value) {
        tft->setTextColor(trueColor);
        int16_t w {tft->textWidth(displayedValueTrue)};
        tft->setCursor(firstLinePosX + lineWidth - w - textIndentation, firstLinePosY + lineHeight * line);
        tft->print(displayedValueTrue);
    }
    else {
        tft-> setTextColor(falseColor);
        int16_t w {tft->textWidth(displayedValueFalse)};
        tft->setCursor(firstLinePosX + lineWidth - w - textIndentation, firstLinePosY + lineHeight * line);
        tft->print(displayedValueFalse);
    }
}

void StatusEntry::setValue(bool b) {
    value = b;
}

void StatusEntry::push() {
    tft->fillRect(firstLinePosX, firstLinePosY + lineHeight * line, lineWidth, lineHeight, bgColor);
    tft->setTextColor(textColor);
    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    tft->setCursor(firstLinePosX + textIndentation, firstLinePosY + lineHeight * line);
    tft->print(displayedEntry);
    pushRightSide();
}


const std::unordered_map<AlarmState, const char*> MainMenuGraph::actionNames = {
    {disarmed,      "arm"},
    {armedHome,     "disarm"},
    {armedAway,     "disarm"},
};

void MainMenuGraph::pushArmDisarmButton() {
    tft->fillRoundRect(
        posX + width/2 - buttonWidth/2,
        posY + height - buttonNegY - buttonHeight/2,
        buttonWidth,
        buttonHeight,
        buttonCornerR,
        armDisarmButtonIsPressed ? buttonColorPressed : buttonColor
    );
    tft->setTextColor(textColor);
    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    int16_t w {tft->textWidth(actionNames.at(System::alarmState))};
    tft->setCursor(posX + width/2 - w/2, posY + height - buttonNegY - buttonHeight/2 + (buttonHeight - textSize * 8) / 2);
    tft->print(actionNames.at(System::alarmState));
}
void MainMenuGraph::pushArmDisarmButton(bool hovering) {
    armDisarmButtonIsPressed = hovering;
    pushArmDisarmButton();
}

const std::unordered_map<AlarmState, const char*> MainMenuGraph::stateNames = {
    {disarmed,      "==== Disarmed ===="},
    {armedHome,     "=== Armed Home ==="},
    {armedAway,     "=== Armed Away ==="},
    {soundAlarm,    "== Sound  Alarm =="},
    {lockdown,      "==== Lockdown ===="}
};

void MainMenuGraph::pushHeader() {
    tft->setTextColor(textColor);
    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    tft->setCursor(posX + width/2 - tft->textWidth(stateNames.at(System::alarmState))/2, posY);
    tft->print(stateNames.at(System::alarmState));
}

void MainMenuGraph::pushArmedHomeButton() {
    static int16_t anchorX {posX + width/2 - largeButtonWidth_submenu/2};
    tft->setTextColor(textColor);
    tft->setTextSize(textSize_submenu);
    tft->setTextFont(textFont_submenu);
    tft->fillRoundRect(
        anchorX,
        posY,
        largeButtonWidth_submenu,
        largeButtonHeight_submenu,
        largeButtonCornerR_submenu,
        armedHomeIsPressed ? buttonColorPressed : buttonColor
    );
    tft->setCursor(
        anchorX + largeButtonWidth_submenu/2 - tft->textWidth("Arm Home")/2,
        posY + (largeButtonHeight_submenu - textSize_submenu * 8) / 2
    );
    tft->print("Arm Home");
}
void MainMenuGraph::pushArmedHomeButton(bool hovering) {
    armedHomeIsPressed = hovering;
    pushArmedHomeButton();
}

void MainMenuGraph::pushArmedAwayButton() {
    static int16_t anchorX {posX + width/2 - largeButtonWidth_submenu/2};
    tft->setTextColor(textColor);
    tft->setTextSize(textSize_submenu);
    tft->setTextFont(textFont_submenu);
    tft->fillRoundRect(
        anchorX,
        posY + largeButtonHeight_submenu + interButtonSpacing_submenu,
        largeButtonWidth_submenu,
        largeButtonHeight_submenu,
        largeButtonCornerR_submenu,
        armedAwayIsPressed ? buttonColorPressed : buttonColor
    );
    tft->setCursor(
        anchorX + largeButtonWidth_submenu/2 - tft->textWidth("Arm Away")/2,
        posY + largeButtonHeight_submenu + interButtonSpacing_submenu + (largeButtonHeight_submenu - textSize_submenu * 8) / 2
    );
    tft->print("Arm Away");
}
void MainMenuGraph::pushArmedAwayButton(bool hovering) {
    armedAwayIsPressed = hovering;
    pushArmedAwayButton();
}

void MainMenuGraph::pushBackButton() {
    static int16_t anchorX {posX + width/2 - largeButtonWidth_submenu/2};
    tft->setTextColor(textColor);
    tft->setTextSize(textSize_submenu);
    tft->setTextFont(textFont_submenu);
    tft->fillRoundRect(
        anchorX,
        posY + 2 * (largeButtonHeight_submenu + interButtonSpacing_submenu),
        largeButtonWidth_submenu,
        smallButtonHeight_submenu,
        largeButtonCornerR_submenu,
        backIsPressed ? buttonColorPressed : buttonColor
    );
    tft->setCursor(
        anchorX + largeButtonWidth_submenu/2 - tft->textWidth("Back")/2,
        posY + 2 * (largeButtonHeight_submenu + interButtonSpacing_submenu) + (smallButtonHeight_submenu - textSize_submenu * 8) / 2
    );
    tft->print("Back");
}
void MainMenuGraph::pushBackButton(bool hovering) {
    backIsPressed = hovering;
    pushBackButton();
}

void MainMenuGraph::begin(DisplayManager* dM, TouchScreenManager* tsM, std::function<void()> startUnlockSequenceCallback) {
    if (!dM || !tsM) return;
    displayManager = dM;
    tsManager = tsM;
    tft = displayManager->getTFT();

    startUnlockSequenceCb = startUnlockSequenceCallback;

    StatusEntry::tft = tft;

    entries[0] = StatusEntry(front_door, 1,     "Front door:",      "Open",     "Closed");
    entries[1] = StatusEntry(window, 2,         "Window:",          "Open",     "Closed");
    entries[2] = StatusEntry(window_sensor, 3,  "Window sensor:",   "Online",   "Offline");
    entries[3] = StatusEntry(camera, 4,         "Camera:",          "Online",   "Offline");
    entries[4] = StatusEntry(wifi, 5,           "Wi-Fi:",           "Online",   "Offline");

    StatusEntry::firstLinePosX = posX;
    StatusEntry::firstLinePosY = posY + headerHeight;
    StatusEntry::lineHeight = lineHeight;
    StatusEntry::lineWidth = width;
    StatusEntry::textColor = textColor;
    StatusEntry::bgColor = bgColor;
    StatusEntry::trueColor = trueColor;
    StatusEntry::falseColor = falseColor;
    StatusEntry::textSize = textSize;
    StatusEntry::textFont = textFont;
    StatusEntry::textIndentation = textIndentation;

    for (int i {}; i < numEntries; i++) {
        setEntryBool(entries[i].getEntryID(), false);
    }

    // armDisarmButton
    TouchRect armDisarmButtonBounds;
    armDisarmButtonBounds.startX = posX + width/2 - buttonWidth/2 - 5;  // -5/+5 to make area easier to click
    armDisarmButtonBounds.startY = posY + height - buttonNegY - buttonHeight/2 - 5;
    armDisarmButtonBounds.endX = posX + width/2 + buttonWidth/2 + 5;
    armDisarmButtonBounds.endY = posY + height - buttonNegY + buttonHeight/2 + 5;
    armDisarmButton = TouchButton(
        armDisarmButtonBounds,
        [this](bool hovering) {
            pushArmDisarmButton(hovering);
        },
        [this]() {
            if (System::alarmState == disarmed) enterSubmenu();
            else if (System::alarmState == armedHome || System::alarmState == armedAway) {
                startUnlockSequenceCb();
            }
        }
    );
    // armedHomeButton
    TouchRect armedHomeButtonBounds;
    armedHomeButtonBounds.startX = posX + width/2 - largeButtonWidth_submenu/2;
    armedHomeButtonBounds.startY = posY;
    armedHomeButtonBounds.endX = posX + width/2 + largeButtonWidth_submenu/2;
    armedHomeButtonBounds.endY = posY + largeButtonHeight_submenu;
    armedHomeButton = TouchButton(
        armedHomeButtonBounds,
        [this](bool hovering) {
            pushArmedHomeButton(hovering);
        },
        [this]() {
            System::alarmState = armedHome;
            exitSubmenu();
        }
    );
    // armedAwayButton
    TouchRect armedAwayButtonBounds;
    armedAwayButtonBounds.startX = posX + width/2 - largeButtonWidth_submenu/2;
    armedAwayButtonBounds.startY = posY + largeButtonHeight_submenu + interButtonSpacing_submenu;
    armedAwayButtonBounds.endX = posX + width/2 + largeButtonWidth_submenu/2;
    armedAwayButtonBounds.endY = posY + 2*largeButtonHeight_submenu + interButtonSpacing_submenu;
    armedAwayButton = TouchButton(
        armedAwayButtonBounds,
        [this](bool hovering) {
            if (hovering) this->pushArmedAwayButton(true);
            else this->pushArmedAwayButton(false);
        },
        [this]() {
            System::alarmState = armedAway;
            exitSubmenu();
        }
    );
    // backButton
    TouchRect backButtonBounds;
    backButtonBounds.startX = posX + width/2 - largeButtonWidth_submenu/2;
    backButtonBounds.startY =posY + 2 * (largeButtonHeight_submenu + interButtonSpacing_submenu);
    backButtonBounds.endX = posX + width/2 + largeButtonWidth_submenu/2;
    backButtonBounds.endY =posY + 2 * (largeButtonHeight_submenu + interButtonSpacing_submenu) + smallButtonHeight_submenu;
    backButton = TouchButton(
        backButtonBounds,
        [this](bool hovering) {
            if (hovering) pushBackButton(true);
            else pushBackButton(false);
        },
        [this]() { exitSubmenu(); }
    );
}


void MainMenuGraph::update() {
    TouchPoint touchPoint {tsManager->getTouch()};
    static uint8_t consecInvalid;
    if (!touchPoint.valid) {
        if (++consecInvalid > 1) return;
    } else {
        consecInvalid = 0;
    }

    if (isInSubmenu) {
        armedHomeButton.checkCollision(touchPoint);
        armedAwayButton.checkCollision(touchPoint);
        backButton.checkCollision(touchPoint);
    } else {
        armDisarmButton.checkCollision(touchPoint);
    }
}

void MainMenuGraph::setAllButtonsFalse() {
    armDisarmButtonIsPressed = false;
    armedHomeIsPressed = false;
    armedAwayIsPressed = false;
    backIsPressed = false;

}

void MainMenuGraph::setEntryBool(StatusScreenEntries entry, bool b) {
    for (int i {}; i < numEntries; i++) {
        if (entries[i].getEntryID() == entry) {
            entries[i].setValue(b);
            break;
        }
    }
}

void MainMenuGraph::pushAll() {
    tft->fillRect(posX, posY, width, height, bgColor);

    if (isInSubmenu) {
        pushArmedHomeButton();
        pushArmedAwayButton();
        pushBackButton();
    } else {
        for (int i {}; i < numEntries; i++) {
            entries[i].push();
        }
        pushHeader();
        pushArmDisarmButton();
    }
}

void MainMenuGraph::enterSubmenu() {
    setAllButtonsFalse();
    isInSubmenu = true;
    pushAll();
}

void MainMenuGraph::exitSubmenu() {
    setAllButtonsFalse();
    isInSubmenu = false;
    pushAll();
}



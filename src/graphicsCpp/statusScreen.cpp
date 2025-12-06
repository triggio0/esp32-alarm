#include "graphicsH/statusScreen.h"
#include "graphicsH/displayManager.h"

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

void MainMenuGraph::pushButton() {
    tft->fillRoundRect(
        posX + width/2 - buttonWidth/2,
        posY + height - buttonNegY - buttonHeight/2,
        buttonWidth,
        buttonHeight,
        buttonCornerR,
        buttonColor
    );
    tft->setTextColor(textColor);
    tft->setTextSize(textSize);
    tft->setTextFont(textFont);
    int16_t w {tft->textWidth(actionNames.at(currentState))};
    tft->setCursor(posX + width/2 - w/2, posY + height - buttonNegY - buttonHeight/2 + (buttonHeight - textSize * 8) / 2);
    tft->print(actionNames.at(currentState));
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
    tft->setCursor(posX + width/2 - tft->textWidth(stateNames.at(currentState))/2, posY);
    tft->print(stateNames.at(currentState));
}

void MainMenuGraph::pushSubmenu() {
    static int16_t anchorX {posX + width/2 - largeButtonWidth_submenu/2};
    tft->setTextColor(textColor);
    tft->setTextSize(textSize_submenu);
    tft->setTextFont(textFont_submenu);
    // armed Home
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
    // armed Away
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
    // back
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

void MainMenuGraph::begin(DisplayManager* dM) {
    if (!dM) return;
    displayManager = dM;
    tft = displayManager->getTFT();

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
}

void MainMenuGraph::setEntryBool(StatusScreenEntries entry, bool b) {
    for (int i {}; i < numEntries; i++) {
        if (entries[i].getEntryID() == entry) {
            entries[i].setValue(b);
            break;
        }
    }
}

void MainMenuGraph::setAlarmState(AlarmState state) {
    currentState = state;
    pushHeader();
    pushButton();
}

void MainMenuGraph::pushAll() {
    tft->fillRect(posX, posY, width, height, bgColor);

    if (isInSubmenu) {
        pushSubmenu();
    } else {
        for (int i {}; i < numEntries; i++) {
            entries[i].push();
        }
        pushHeader();
        pushButton();
    }
}

void MainMenuGraph::enterSubmenu() {
    isInSubmenu = true;
    pushAll();
}

void MainMenuGraph::exitSubmenu() {
    isInSubmenu = false;
    pushAll();
}

void MainMenuGraph::pressMainButton(bool pressed) {
    mainButtonIsPressed = pressed;
    pushButton();
}

void MainMenuGraph::pressSubmenuButton(int8_t button, bool pressed) {
    switch (button) {
        case 0:
            armedHomeIsPressed = pressed;
            break;
        case 1:
            armedAwayIsPressed = pressed;
            break;
        case 2:
            backIsPressed = pressed;
            break;
        default:
            break;
    }
    pushSubmenu();
}
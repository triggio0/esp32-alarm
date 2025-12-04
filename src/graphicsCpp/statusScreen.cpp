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



void StatusScreenGraph::begin(DisplayManager* dM) {
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

void StatusScreenGraph::setEntryBool(StatusScreenEntries entry, bool b) {
    for (int i {}; i < numEntries; i++) {
        if (entries[i].getEntryID() == entry) {
            entries[i].setValue(b);
            break;
        }
    }
}

void StatusScreenGraph::pushAll() {
    tft->fillRect(posX, posY, width, height, bgColor);

    for (int i {}; i < numEntries; i++) {
        entries[i].push();
    }
}
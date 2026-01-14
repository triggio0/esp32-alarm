#include "config/pinLayout.h"
#include "globals.h"
#include <Arduino.h>


class ReedManager {
private:
    static constexpr uint16_t debounceDelay {20};

    bool debouncedValue {LOW};
    bool lastReading {HIGH};
    unsigned long lastChangeTime {};    

    void warmup() {
        for (int i {}; i<20; i++) {
            update();
        }
    }

public:
    ReedManager() {
        pinMode(PinLayout::reedDoor, INPUT_PULLUP);
        warmup();
    }

    void update() {
        bool reading = digitalRead(PinLayout::reedDoor);
        if (reading != lastReading) {
        lastChangeTime = millis(); 
    }

    if ((millis() - lastChangeTime) > debounceDelay) {
        if (reading != debouncedValue) {
            debouncedValue = reading;
            System::doorOpen = debouncedValue;
        }
    }
    lastReading = reading;
    }

    bool doorIsOpen() { return (debouncedValue == HIGH); }
};

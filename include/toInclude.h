#pragma once
#include <Arduino.h>
#include <chrono>
#include <WiFi.h>
#include <esp_now.h>

namespace PinLayout {
    constexpr uint8_t ledRGB {48};
    
    constexpr uint8_t kpRow1 {14};
    constexpr uint8_t kpRow2 {16};
    constexpr uint8_t kpRow3 {17};
    constexpr uint8_t kpRow4 {18};
    constexpr uint8_t kpCol1 {19};
    constexpr uint8_t kpCol2 {20};
    constexpr uint8_t kpCol3 {21};

    constexpr uint8_t reedDoor {8};

    constexpr uint8_t buzzer {15};
    constexpr uint8_t buzzerChannel {0};
}

enum AlarmState {
    /*
    
    */disarmed,/*      -> sensors and camera inactive
        - submenu for armed type
        -> amedHome, armedAway
    */armedHome,/*     -> sensors active
        -> disarmed, alarm
    */armedAway,/*     -> sensors active
        -> disarmed, lockdown
    */soundAlarm,/*         -> sounds alarm to wake me up (no pin request)
        -> disarmed
    */lockdown,/*      -> camera active, asks pin at first. Used if away for long (must go back to armed autonomously)
        -> disarmed, armedAway
    
    */
};
#pragma once
#include <stdint.h>

namespace PinLayout {
    constexpr int16_t ledRGB {48};
    
    // constexpr uint8_t kpRow1 {14};
    // constexpr uint8_t kpRow2 {16};
    // constexpr uint8_t kpRow3 {17};
    // constexpr uint8_t kpRow4 {18};
    // constexpr uint8_t kpCol1 {19};
    // constexpr uint8_t kpCol2 {20};
    // constexpr uint8_t kpCol3 {21};

    constexpr int16_t reedDoor {8};

    constexpr int16_t buzzer {15};
    constexpr int16_t buzzerChannel {0};

    // following need to be changed in platformio.ini too
    constexpr int16_t tftMiso {19};
    constexpr int16_t tftMosi {23};
    constexpr int16_t tftSclk {18};
    constexpr int16_t tftCs {15};
    constexpr int16_t tftDc {2};
    constexpr int16_t tftRst {4};

    constexpr int16_t tftBacklight {5};
    constexpr int16_t backlightChannel {1};
}
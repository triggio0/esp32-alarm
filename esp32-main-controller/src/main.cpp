#include <Arduino.h>
#include <WiFi.h>
#include "utils.h"
#include "globals.h"

void maintainWiFiTask(void *param);
void mainLoopTask(void *param);

AlarmState System::alarmState = armedHome;
PowerState System::powerState = active;

void setup() {

    Serial.begin(115200);
    delay(1000);
    Serial.println("================   SETUP   ================\n");

    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024*1024));
    Serial.printf("PSRAM: %d MB\n", ESP.getPsramSize() / (1024*1024));
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Starting used heap: %.1f%%\n", getHeapUsedPercent());
    Serial.println("===========================================\n");



    // xTaskCreatePinnedToCore(
    //     maintainWiFiTask,
    //     "WiFi",
    //     4096,
    //     NULL,
    //     1,
    //     NULL,
    //     0
    // );

    xTaskCreatePinnedToCore(
        mainLoopTask,
        "main loop",
        4096,
        NULL,
        1,
        NULL,
        1
    );

}

void loop() {
    delay(1000);
}
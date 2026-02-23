#include <Arduino.h>
#include <WiFi.h>
#include "utils.h"
#include "globals.h"
#include "config/secret.h"

void maintainWiFiTask(void *param);
void mainLoopTask(void *param);

AlarmState System::alarmState {disarmed};
PowerState System::powerState {active};
bool System::isOnline {false};
bool System::doorOpen {false};
uint32_t System::startUptime {};

void setup() {

    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(1000));
    System::startUptime = millis();

    Serial.println("================   SETUP   ================\n");
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024*1024));
    Serial.printf("PSRAM: %d MB\n", ESP.getPsramSize() / (1024*1024));
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Starting used heap: %.1f%%\n", getHeapUsedPercent());
    Serial.println("===========================================\n");

    xTaskCreatePinnedToCore(
        mainLoopTask,
        "main loop",
        16384,
        NULL,
        1,
        NULL,
        1
    );

    if (WiFiSsid != "") {
        xTaskCreatePinnedToCore(
            maintainWiFiTask,
            "WiFi",
            8192,
            NULL,
            1,
            NULL,
            0
        );
    }
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    // Serial.printf("Free internal heap: %d bytes\n", 
    //     heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // Serial.printf("Largest free block: %d bytes\n", 
    //     heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));

}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
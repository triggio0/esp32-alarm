#include "toInclude.h"
#include "utils.h"




void maintainWiFiTask(void *param);
void mainLoopTask(void *param);

void setup() {

    Serial.begin(115200);
    delay(5000);
    Serial.println("================   SETUP   ================\n");

    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024*1024));
    Serial.printf("PSRAM: %d MB\n", ESP.getPsramSize() / (1024*1024));
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Used heap: %f%", getHeapUsedPercent());
    Serial.println("===========================================\n");



    xTaskCreatePinnedToCore(
        maintainWiFiTask,
        "WiFi",
        4096,
        NULL,
        1,
        NULL,
        0
    );

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
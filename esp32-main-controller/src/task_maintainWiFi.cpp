#include <WiFi.h>
#include "config/secret.h"
#include "globals.h"

void syncTime() {
    struct tm timeinfo;
    int retry {0};
    const int maxRetries {20};
    Serial.println("|  maintainWiFiTask  |> Attempting to sync time... ");
    while (!getLocalTime(&timeinfo) && retry < maxRetries) {
        delay(1000);
        retry++;
    }

    if (retry < maxRetries) {
        Serial.print("|  maintainWiFiTask  |> Time synchronized - ");
        Serial.printf("Current time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    } else {
        Serial.println("|  maintainWiFiTask  |> Failed to sync time");
    }
}

bool attemptConnection() {
    WiFi.disconnect();
    WiFi.begin(WiFiSsid, WiFiPassword);
    unsigned long startAttempt {millis()};
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 5*1000) {
        delay(500);
    }
    return (WiFi.status() == WL_CONNECTED);
}

void maintainWiFiTask(void *param) {
    Serial.println("|  maintainWiFiTask  |> Task created ");

    const unsigned long reconnectInterval {1000 * 60};
    const unsigned long syncTimeInterval {1000 * 60 * 60 * 24};
    
    Serial.println("|  maintainWiFiTask  |> Connecting to WiFi...");
    WiFi.mode(WIFI_STA);

    bool connected {attemptConnection()};
    
    if (connected) {
        Serial.println("|  maintainWiFiTask  |> WiFi connected!");
        Serial.print("|  maintainWiFiTask  |> IP: ");
        Serial.println(WiFi.localIP());
        System::isOnline = true;
    } else {
        Serial.println("\n|  maintainWiFiTask  |> Initial connection failed, will retry...");
    }

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    syncTime();
    
    unsigned long lastReconnectAttempt {0};
    unsigned long lastSyncAttempt {0};
    
    while (true) {
        uint32_t now = millis();
        if (WiFi.status() != WL_CONNECTED) {
            System::isOnline = false;
            if (now - lastReconnectAttempt >= reconnectInterval) {
                Serial.print("|  maintainWiFiTask  |> Attempting to reconnect...");
                if (attemptConnection()) {
                    System::isOnline = true;
                    Serial.println(" Success!");
                } else {
                    Serial.println(" Fail!");
                }
                lastReconnectAttempt = now;
            }
        } else {
            static unsigned long lastPrint {};
            if (now - lastPrint >= 10*60*1000) {
                Serial.print("|  maintainWiFiTask  |> WiFi still connected, RSSI: ");
                Serial.println(WiFi.RSSI());
                lastPrint = now;

            if (now - lastSyncAttempt >= syncTimeInterval) {
                syncTime();
                lastSyncAttempt = now;
            }
            }
            
        }
        delay(1000);
    }
}
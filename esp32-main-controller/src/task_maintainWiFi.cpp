#include "toInclude.h"
#include "config/secret.h"

void syncTime() {
    struct tm timeinfo;
    int retry {0};
    const int maxRetries {5};
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

void maintainWiFiTask(void *param) {
    Serial.println("|  maintainWiFiTask  |> Task created ");

    const unsigned long reconnectInterval {1000 * 30};
    const unsigned long syncTimeInterval {1000 * 60 * 60 * 24};
    
    Serial.println("|  maintainWiFiTask  |> Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WiFiSsid, WiFiPassword);
    
    unsigned long startAttempt {millis()};
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 30000) {
        delay(500);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("|  maintainWiFiTask  |> WiFi connected!");
        Serial.print("|  maintainWiFiTask  |> IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n|  maintainWiFiTask  |> Initial connection failed, will retry...");
    }

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    syncTime();
    
    unsigned long lastReconnectAttempt {0};
    unsigned long lastSyncAttempt {0};
    
    while (true) {
        if (WiFi.status() != WL_CONNECTED) {
            
            if (millis() - lastReconnectAttempt >= reconnectInterval) {
                Serial.println("|  maintainWiFiTask  |> Attempting to reconnect...");
                WiFi.disconnect();
                WiFi.begin(WiFiSsid, WiFiPassword);
                lastReconnectAttempt = millis();
            }
        } else {
            static unsigned long lastPrint {0};
            if (millis() - lastPrint >= 600000) {
                Serial.print("|  maintainWiFiTask  |> WiFi still connected, RSSI: ");
                Serial.println(WiFi.RSSI());
                lastPrint = millis();

            if (millis() - lastSyncAttempt >= syncTimeInterval) {
                syncTime();
                lastSyncAttempt = millis();
            }
            }
            
        }
        delay(1000);
    }
}
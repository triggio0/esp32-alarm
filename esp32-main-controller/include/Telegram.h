#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config/secret.h"

#include <HTTPClient.h>

class TelegramBot {
private:
    WiFiClientSecure client;
    UniversalTelegramBot* bot;
    // const const char* BotToken {BOTtoken};
    // const const char* chat_id {CHAT_ID}; 
    static constexpr int16_t botRequestDelay {5 * 1000};
    uint32_t lastTimeBotRan;
    bool init {false};
    bool active {true};

    void handleIncoming(int16_t numNewMessages) {
        for (int i=0; i<numNewMessages; i++) {

            String chat_id = String(bot->messages[i].chat_id);
            if (chat_id != CHAT_ID) {
                String msg ="Unauthorized user: id= " + chat_id;
                bot->sendMessage(chat_id, msg, "");
                continue;
            }

            String text = bot->messages[i].text;
            Serial.println(text);
            String from_name = bot->messages[i].from_name;

            if (text == "/start") {
                String msg =    "Welcome " + from_name + ".\n"
                                "Use any of the following commands:\n"
                                "-> /report\n"
                                "-> /armHome\n"
                                "-> /armAway\n"
                                "";
                bot->sendMessage(chat_id, msg, "");
            } else if (text == "/report") {
                uint32_t totalHours = (millis() - System::startUptime) / 1000 / 60 / 60;
                uint16_t days = totalHours / 24;
                uint16_t hours = totalHours % 24;
                String msg =    "=============== report ===============\n"
                                "Uptime:" + String(days) + "d, " + String(hours) + "h\n"
                                "Window sensor: \n"
                                "Camera: \n";
                bot->sendMessage(chat_id, msg, "");
            }

        }
    }

public:
    TelegramBot() : bot(nullptr) {}

    void sendMessage(const char* message){
        bot->sendMessage(CHAT_ID, message, "");
    }

    void begin() {
        Serial.println("Telegram: Initializing...");

        client.setInsecure();
        bot = new UniversalTelegramBot(BOTtoken, client);

        bot->sendMessage(CHAT_ID, "esp32 online!", "");


        // Serial.printf("Bot Token: %s\n", BOTtoken);
        // Serial.printf("Chat ID: %s\n", CHAT_ID);
        // Serial.printf("WiFi Status: %d\n", WiFi.status());
        // Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        
        // client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
        // // client.setInsecure();
        // bot = new UniversalTelegramBot(BOTtoken, client);

        // // Test HTTPS connection
        // Serial.println("Testing HTTPS connection to Telegram...");
        // WiFiClientSecure testClient;
        // testClient.setInsecure();
        
        // if (testClient.connect("api.telegram.org", 443)) {
        //     Serial.println("✅ Connected to api.telegram.org");
        //     testClient.stop();
        // } else {
        //     Serial.println("❌ Failed to connect to api.telegram.org");
        //     testClient.stop();
        //     return;
        // }
        
        // // Test manual API call
        // Serial.println("Testing manual sendMessage API call...");
        // HTTPClient http;
        // String url = "https://api.telegram.org/bot" + String(BOTtoken) + "/sendMessage";
        // String payload = "{\"chat_id\":\"" + String(CHAT_ID) + "\",\"text\":\"🤖 Test message\"}";
        // http.begin(url);
        // http.addHeader("Content-Type", "application/json");
        // int httpCode = http.POST(payload);
        // String response = http.getString();
        // Serial.printf("HTTP Code: %d\n", httpCode);
        // Serial.printf("Response: %s\n", response.c_str());
        // http.end();
        
        // // Enable debug mode in UniversalTelegramBot
        // Serial.println("\nTesting bot->sendMessage with library debug...");
        
        // // Check if bot pointer is valid
        // if (!bot) {
        //     Serial.println("❌ Bot pointer is NULL!");
        //     return;
        // }
        // Serial.println("✅ Bot pointer is valid");
        
        // // Check client connection state
        // Serial.printf("Client connected: %d\n", client.connected());
        
        // // Try sending with the library
        // String testMsg = "🤖 Library test";
        // Serial.printf("Calling bot->sendMessage(\"%s\", \"%s\", \"\")\n", CHAT_ID, testMsg.c_str());
        
        // bool sent = bot->sendMessage(CHAT_ID, testMsg, "");
        
        // Serial.printf("Result: %s\n", sent ? "✅ SUCCESS" : "❌ FAILED");
        
        // // If failed, check what might be wrong
        // if (!sent) {
        //     Serial.println("\nDiagnosing failure...");
        //     Serial.printf("Bot last_message_received: %d\n", bot->last_message_received);
            
        //     // Try with String instead of const char*
        //     Serial.println("Trying with String chat_id...");
        //     String chatIdStr = String(CHAT_ID);
        //     bool sent2 = bot->sendMessage(chatIdStr, testMsg, "");
        //     Serial.printf("Result: %s\n", sent2 ? "✅ SUCCESS" : "❌ FAILED");
        // }

        init = true;
    }


    void update() {
        if (!active) return;
        if (!System::isOnline) {
            // init = false;            // TODO: check if needed
            return;
        }
        if (!init) {
            begin();
        }
        uint32_t now {millis()};
        if (now < lastTimeBotRan + botRequestDelay) return;
        
        // // Debug: Print status every 10 seconds
        // static uint32_t lastDebug = 0;
        // if (now - lastDebug > 100000) {
        //     Serial.printf("Telegram: WiFi status=%d, checking messages (last_msg=%d)\n", 
        //                 WiFi.status(), bot->last_message_received);
        //     lastDebug = now;
        // }
        
        int16_t numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        
        if (numNewMessages > 0) {
            handleIncoming(numNewMessages);
            numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        } else if (numNewMessages < 0) {
            Serial.println("Telegram getUpdates() error");
        }
        
        lastTimeBotRan = now;

    }

    void setOn () { active = true; }
    void setOff () { active = false; }
};











// /*
//   Rui Santos
//   Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
//   Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
//   Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
// */

// #ifdef ESP32
//   #include <WiFi.h>
// #else
//   #include <ESP8266WiFi.h>
// #endif
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
// #include <ArduinoJson.h>

// // Replace with your network credentials
// const char* ssid = "REPLACE_WITH_YOUR_SSID";
// const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// // Initialize Telegram BOT
// #define BOTtoken "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

// // Use @myidbot to find out the chat ID of an individual or a group
// // Also note that you need to click "start" on a bot before it can
// // message you
// #define CHAT_ID "XXXXXXXXXX"

// #ifdef ESP8266
//   X509List cert(TELEGRAM_CERTIFICATE_ROOT);
// #endif

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOTtoken, client);

// // Checks for new messages every 1 second.
// int botRequestDelay = 1000;
// unsigned long lastTimeBotRan;

// const int ledPin = 2;
// bool ledState = LOW;

// // Handle what happens when you receive new messages
// void handleNewMessages(int numNewMessages) {
//   Serial.println("handleNewMessages");
//   Serial.println(String(numNewMessages));

//   for (int i=0; i<numNewMessages; i++) {
//     // Chat id of the requester
//     String chat_id = String(bot.messages[i].chat_id);
//     if (chat_id != CHAT_ID){
//       bot.sendMessage(chat_id, "Unauthorized user", "");
//       continue;
//     }
    
//     // Print the received message
//     String text = bot.messages[i].text;
//     Serial.println(text);

//     String from_name = bot.messages[i].from_name;

//     if (text == "/start") {
//       String welcome = "Welcome, " + from_name + ".\n";
//       welcome += "Use the following commands to control your outputs.\n\n";
//       welcome += "/led_on to turn GPIO ON \n";
//       welcome += "/led_off to turn GPIO OFF \n";
//       welcome += "/state to request current GPIO state \n";
//       bot.sendMessage(chat_id, welcome, "");
//     }

//     if (text == "/led_on") {
//       bot.sendMessage(chat_id, "LED state set to ON", "");
//       ledState = HIGH;
//       digitalWrite(ledPin, ledState);
//     }
    
//     if (text == "/led_off") {
//       bot.sendMessage(chat_id, "LED state set to OFF", "");
//       ledState = LOW;
//       digitalWrite(ledPin, ledState);
//     }
    
//     if (text == "/state") {
//       if (digitalRead(ledPin)){
//         bot.sendMessage(chat_id, "LED is ON", "");
//       }
//       else{
//         bot.sendMessage(chat_id, "LED is OFF", "");
//       }
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);

//   #ifdef ESP8266
//     configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
//     client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
//   #endif

//   pinMode(ledPin, OUTPUT);
//   digitalWrite(ledPin, ledState);
  
//   // Connect to Wi-Fi
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   #ifdef ESP32
//     client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
//   #endif
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi..");
//   }
//   // Print ESP32 Local IP Address
//   Serial.println(WiFi.localIP());
// }

// void loop() {
//   if (millis() > lastTimeBotRan + botRequestDelay)  {
//     int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

//     while(numNewMessages) {
//       Serial.println("got response");
//       handleNewMessages(numNewMessages);
//       numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//     }
//     lastTimeBotRan = millis();
//   }
// }
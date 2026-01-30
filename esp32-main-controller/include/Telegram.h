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
                uint32_t totalMinutes = (millis() - System::startUptime) / 1000 / 60;
                uint16_t days    = totalMinutes / (24 * 60);
                uint16_t hours   = (totalMinutes % (24 * 60)) / 60;
                uint16_t minutes = totalMinutes % 60;

                String msg =    "=============== report ===============\n"
                                "Uptime: " + String(days) + "d, " + String(hours) + "h, " + String(minutes) + "m\n"
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
        init = true;
    }


    void update() {
        if (!active) return;
        if (!System::isOnline) {
            return;
        }
        if (!init) {
            begin();
        }
        uint32_t now {millis()};
        if (now < lastTimeBotRan + botRequestDelay) return;
        
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
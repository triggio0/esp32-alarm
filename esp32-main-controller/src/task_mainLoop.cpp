#include <Arduino.h>
#include "Ui_headers/UiManager.h"
#include "Telegram.h"
#include "reedManager.h"
#include "buzzerManager.h"

constexpr int MAX_FRAMERATE {16};

UiManager uiManager;
TelegramBot tgBot;
ReedManager reedManager;
BuzzerManager buzzerManager;


void alarmTrigger() {
    if (System::alarmState == armedHome) {
        System::alarmState = soundAlarm;
        buzzerManager.beep(1000);
    } else if (System::alarmState == armedAway) {
        System::alarmState = lockdown;
    } else {
        Serial.println("Error - alarmTrigger() but state was not armed!");
        return;
    }

    tgBot.sendMessage("⚠️ Alarm triggered! ⚠️");
    
    // TODO: continue here
}


void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");
    uiManager.begin();
    tgBot = TelegramBot();
    uiManager.setFpsOptimizationCb(
        []() { tgBot.setOff(); },
        []() { tgBot.setOn(); }
    );
    uiManager.setAlarmTriggerCb(alarmTrigger);
    reedManager = ReedManager();
    buzzerManager = BuzzerManager();
    // uiManager.setBuzzerManager(&buzzerManager);

    const TickType_t frameDelay = pdMS_TO_TICKS( static_cast<int>( 1000 / MAX_FRAMERATE ) );
    TickType_t lastWakeTime = xTaskGetTickCount();

    Serial.printf("used heap: %.1f%%\n", getHeapUsedPercent());

    int count {0};
    while (true) {
        uiManager.update();
        tgBot.update();
        reedManager.update();
        if (System::doorOpen && (System::alarmState == armedHome || System::alarmState == armedAway)) {
            alarmTrigger();
        }
        buzzerManager.update();




        // count++;
        // if (count == 500) {
        //     Serial.printf("- %.0f\n", getFpsCount());
        //     count = 0;
        // } else {
        //     getFpsCount();
        // }
        vTaskDelayUntil(&lastWakeTime, frameDelay);
    }
}
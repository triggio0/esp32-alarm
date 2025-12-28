#include "toInclude.h"
#include "TFT_h/displayManager.h"
#include "TFT_h/statusScreen.h"
#include "TFT_h/touchScreenManager.h"
#include "TFT_h/keypad.h"
#include "TFT_h/eye.h"

DisplayManager displayManager;
MainMenuGraph mainMenuGraph;
KeypadGraph keypadGraph;
EyeSprite eyeSprite;
TouchScreenManager touchScreenManager;








void manageState() {

    eyeSprite.update();
    eyeSprite.push();           // TODO: optimize, way too slow!!
    
    touchScreenManager.update();
    mainMenuGraph.update();


    return;
}


void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");
    // begin 
    displayManager.begin();
    displayManager.fillColor(TFT_WHITE);
    touchScreenManager.begin(displayManager.getTFT());
    keypadGraph.begin(&displayManager, &touchScreenManager);
    mainMenuGraph.begin(&displayManager, &touchScreenManager);
    mainMenuGraph.setAlarmState(alarmState);
    mainMenuGraph.pushAll();
    eyeSprite.begin(&displayManager);
    eyeSprite.push(true);
    
    // FPS counter variables
    uint32_t frameCount = 0;
    uint32_t lastFpsTime = millis();
    uint32_t totalWorkTime = 0;  // Track actual work time
    
    while (true) {
        uint32_t frameStart = millis();
        
        manageState();
        
        uint32_t frameEnd = millis();
        uint32_t workTime = frameEnd - frameStart;
        totalWorkTime += workTime;
        
        // FPS calculation
        frameCount++;
        uint32_t currentTime = millis();
        
        // Print FPS every 5 seconds
        if (currentTime - lastFpsTime >= 1000) {
            uint32_t totalElapsed = currentTime - lastFpsTime;
            float fps = frameCount * 1000.0 / totalElapsed;
            float frameTime = totalElapsed / (float)frameCount;
            float cpuUsage = (totalWorkTime * 100.0) / totalElapsed;
            
            Serial.printf("FPS: %.1f | Frame time: %.1f ms | CPU usage: %.1f%%\n", 
                         fps, frameTime, cpuUsage);
            
            frameCount = 0;
            totalWorkTime = 0;
            lastFpsTime = currentTime;
        }
        
        delay(0);
    }
}

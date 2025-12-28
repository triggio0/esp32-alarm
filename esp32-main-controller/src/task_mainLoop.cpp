#include "toInclude.h"



void mainLoopTask(void *param) {
    Serial.println("|    mainLoopTask    |> Task created");

    uint32_t frameCount = 0;
    uint32_t lastFpsTime = millis();
    uint32_t totalWorkTime = 0;  // Track actual work time
    
    while (true) {
        uint32_t frameStart = millis();
                
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

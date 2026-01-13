#pragma once


enum AlarmState {
    /*
    
    */disarmed,/*      -> sensors and camera inactive
        - submenu for armed type
        -> amedHome, armedAway
    */armedHome,/*     -> sensors active
        -> disarmed, alarm
    */armedAway,/*     -> sensors active
        -> disarmed, lockdown
    */soundAlarm,/*         -> sounds alarm to wake me up (no pin request)
        -> disarmed
    */lockdown,/*      -> camera active, asks pin at first. Used if away for long (must go back to armed autonomously)
        -> disarmed, armedAway
    
    */
};

enum PowerState {
    active,
    idleDim,
    powerSaving,
};

enum UnlockSequenceState {
    none,
    inProgress,
    codeSuccess,
    codeFail,
    codeAbort,
};

namespace System {
    extern AlarmState alarmState;
    extern PowerState powerState;
    extern uint32_t startUptime;
    extern bool isOnline;
    extern bool doorOpen;
}
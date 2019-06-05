#include "defs.h"

#include "blinker.h"
#include "timer.h"

void Blink(void* param)
{
    Blinker_Toggle();
}

void Init(void)
{
    sei();
    
    Timer_Init();
    Blinker_Init();

    TimerCallbackInfo info;
    info.m_func = Blink;
    info.m_param = NULL;
    info.m_persistent = true;
    Timer_ScheduleCallback(1000, TimerUnits_kMiliseconds, &info);
}

void Tick(void)
{
    // Blinker_Toggle();
    // Timer_StaticDelayForMs(1000);
}

int main(void)
{
    Init();
    while(true)
    {
        Tick();
    }

    return 0;
}
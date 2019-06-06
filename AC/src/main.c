#include "defs.h"

#include "blinker.h"
#include "timer.h"
#include "uart.h"


void Blink(void* param)
{
    Blinker_Toggle();
}

void Init(void)
{
    sei();
    
    Timer_Init();
    Blinker_Init();
    Uart_Init();

    TimerCallbackInfo info;
    info.m_func = Blink;
    info.m_param = NULL;
    info.m_persistent = true;
    Timer_ScheduleCallback(1000, TimerUnits_kMiliseconds, &info);

    Uart_Print("Dupa!\n");
}

void Tick(void)
{
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

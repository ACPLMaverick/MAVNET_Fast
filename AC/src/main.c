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

    Timer_ScheduleCallbackMs_2(1000, Blink, NULL, TIMER_CALL_NUM_PERSISTENT);
}

void Tick(void)
{
    static uint16_t ctr = 0;

    Uart_Printf("Dupa! %d \n", ctr);
    Blinker_Toggle();
    Timer_SleepMs(1000);

    ++ctr;
}

int main(void)
{
    Init();
    while(true)
    {
        //Tick();
    }

    return 0;
}

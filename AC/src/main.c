#include "defs.h"

#include "blinker.h"
#include "timer.h"
#include "uart.h"
#include "display.h"


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
    Disp_Init();

    Timer_ScheduleCallbackMs_2(1000, Blink, NULL, TIMER_CALL_NUM_PERSISTENT);

    Disp_Print("Hello world!", Disp_Row_kUpper, Disp_Alignment_kLeft);
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

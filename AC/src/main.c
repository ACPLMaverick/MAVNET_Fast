#include "defs.h"

#include "blinker.h"
#include "timer.h"
#include "uart.h"
#include "display.h"


void Blink(void* param)
{
    Blinker_Toggle_0();

    static uint16_t ctr = 0;
    Disp_Alignment alignment = (Disp_Alignment)(ctr % 3);
    Disp_ClearRow(Disp_Row_kLower);
    Disp_Printf(Disp_Row_kLower, alignment, "Hello: %d", ctr);

    ++ctr;
}

void Init(void)
{
    // ++lib
    sei();
    
    Timer_Init();
    Blinker_Init();
    Uart_Init();
    Disp_Init();
    // --lib

    Disp_On();
    Disp_Print(Disp_Row_kUpper, Disp_Alignment_kCenter, "S.T.A.L.K.E.R.");

    Timer_ScheduleCallbackMs_2(1000, Blink, NULL, TIMER_CALL_NUM_PERSISTENT);
}

void Tick(void)
{
    // ++lib
    Disp_Tick();
    // --lib
}

int main(void)
{
    // ++lib
    Init();
    while(true)
    {
        Tick();
    }
    // --lib

    return 0;
}

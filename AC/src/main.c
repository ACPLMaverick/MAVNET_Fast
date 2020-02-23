#include "avrlib.h"

void Blink(void* param)
{
    //Lib_Blinker_Toggle_0();

    static uint16_t ctr = 0;
    Lib_Disp_Alignment alignment = (Lib_Disp_Alignment)(ctr % 3);
    Lib_Disp_ClearRow(Lib_Disp_Row_kLower);
    Lib_Disp_Printf(Lib_Disp_Row_kLower, alignment, "Hello: %d", ctr);

    ++ctr;
}

void Init(void)
{
    Lib_Disp_On();
    Lib_Disp_Print(Lib_Disp_Row_kUpper, Lib_Disp_Alignment_kCenter, "CLIMATRONIC");

    Lib_Timer_ScheduleCallbackMs_2(1000, Blink, NULL, LIB_TIMER_CALL_NUM_PERSISTENT);

    Lib_Blinker_Toggle_0();
    Lib_Pwm_Set_1A(UINT16_MAX / 4);
    Lib_Pwm_Set_1B(UINT16_MAX / 16);
}

void Tick(void)
{

}

LIB_ENTRY_POINT(Init, Tick)
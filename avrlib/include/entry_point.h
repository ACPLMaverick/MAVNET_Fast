#pragma once

#if LIB_USE_BLINKER
#include "blinker.h"
#endif
#if LIB_USE_TIMER
#include "timer.h"
#endif
#if LIB_USE_UART
#include "uart.h"
#endif
#if LIB_USE_DISPLAY
#include "display.h"
#endif


inline void Lib_Init(void)
{
    sei();
    
#if LIB_USE_BLINKER
    Lib_Blinker_Init();
#endif
#if LIB_USE_TIMER
    Lib_Timer_Init();
#endif
#if LIB_USE_UART
    Lib_Uart_Init();
#endif
#if LIB_USE_DISPLAY
    Lib_Disp_Init();
#endif
}

inline void Lib_Tick(void)
{
#if LIB_USE_DISPLAY
    Lib_Disp_Tick();
#endif
}

#define LIB_ENTRY_POINT(initFunc, tickFunc)             \
int main(void)                                          \
{                                                       \
    Lib_Init();                                         \
    initFunc();                                         \
    while(true)                                         \
    {                                                   \
        Lib_Tick();                                     \
        tickFunc();                                     \
    }                                                   \
}

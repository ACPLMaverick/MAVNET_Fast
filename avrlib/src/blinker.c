#include "blinker.h"

void Lib_Blinker_Init(void)
{
    for(uint8_t i = 0; i < LIB_BLINKER_NUM_PINS; ++i)
    {
        Lib_BitEnable(LIB_BLINKER_DDR, i);
    }
}
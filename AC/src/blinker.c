#include "blinker.h"

void Blinker_Init(void)
{
    for(uint8_t i = 0; i < BLINKER_NUM_PINS; ++i)
    {
        BitEnable(BLINKER_DDR, i);
    }
}
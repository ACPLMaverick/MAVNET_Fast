#include "blinker.h"

void Blinker_Init(void)
{
    BitEnable(BLINKER_REG, BLINKER_PIN);
}

void Blinker_On(void)
{
    BitEnable(BLINKER_PORT, BLINKER_PIN);
}

void Blinker_Off(void)
{
    BitDisable(BLINKER_PORT, BLINKER_PIN);
}

void Blinker_Toggle(void)
{
    BitToggle(BLINKER_PORT, BLINKER_PIN);
}
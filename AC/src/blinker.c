#include "blinker.h"

#define BLINKER_REG DDRC
#define BLINKER_PORT PORTC
#define BLINKER_PIN PORTC0

void Blinker_Init(void)
{
    BLINKER_REG = (1 << BLINKER_PIN);
}

void Blinker_On(void)
{
    BLINKER_PORT |= (1 << BLINKER_PIN);
}

void Blinker_Off(void)
{
    BLINKER_PORT &= ~(1 << BLINKER_PIN);
}

void Blinker_Toggle(void)
{
    BLINKER_PORT ^= (1 << BLINKER_PIN);
}
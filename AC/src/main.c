#include "defs.h"

#include "blinker.h"
#include "timer.h"

void Init(void)
{
    sei();
    
    Timer_Init();
    Blinker_Init();
}

void Tick(void)
{
    Blinker_Toggle();
    Timer_StaticDelayForMs(1000);
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
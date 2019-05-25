#include "defs.h"

int main(void)
{
    DDRB = 0xFF;    // Makes PORTB as Output.
    while(true)
    {
        PORTB ^= 0xFF;
        _delay_ms(1000);
    }

    return 0;   // Should never get here.
}
#include "uart.h"

// ////////////////////////

#include "timer.h"
#include <stdio.h>

#define BAUDRATE ((F_CPU)/(BAUD*16ULL)-1ULL)    // set baudrate value for UBRR

// ////////////////////////

#define PRINTF_BUFFER_SIZE 256
static char g_printfBuffer[PRINTF_BUFFER_SIZE];

// ////////////////////////

void Uart_Init(void)
{
    g_printfBuffer[0] = 0;

    RegWrite16(UBRRL, UBRRH, BAUDRATE);                 // Set baud rate.
    RegWrite(UCSRB, (1<<TXEN) | (1<<RXEN));               // Enable receiver and transmitter.
    RegWrite(UCSRC, (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));  // 8bit data format, all other settings go as default.
}

void Uart_SendNow(uint8_t byte)
{
    BitWait(UCSRA, UDRE, 0);            // Wait while register is free.
    RegWrite(UDR, byte);

    // Support for endline char carriage return.
    if(byte == '\n')
    {
        Uart_SendNow('\r');
    }
}

void Uart_SendData(uint8_t* buffer, uint16_t size)
{
    for(uint16_t i = 0; i < size; ++i)
    {
        Uart_SendNow(buffer[i]);
    }
}

void Uart_Printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(g_printfBuffer, PRINTF_BUFFER_SIZE, format, args);

    const uint16_t printBufferLength = strnlen(g_printfBuffer, PRINTF_BUFFER_SIZE - 1);

    Uart_SendData((uint8_t*)g_printfBuffer, printBufferLength);

    va_end(args);
}
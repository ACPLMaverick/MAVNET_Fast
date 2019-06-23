#include "uart.h"

// ////////////////////////

#include "timer.h"
#include <stdio.h>

#define UART_BAUDRATE ((F_CPU)/(BAUD*16ULL)-1ULL)    // set baudrate value for UBRR

// ////////////////////////

#define UART_ASYNC_TRANSMIT 1

#define UART_PRINTF_BUFFER_SIZE UINT8_MAX
static char g_printfBuffer[UART_PRINTF_BUFFER_SIZE];

#define UART_RECEIVE_BUFFER_SIZE UINT8_MAX
static uint8_t g_receiveBuffer[UART_RECEIVE_BUFFER_SIZE];
static uint8_t g_receiveBufferIndex = 0;

#if UART_ASYNC_TRANSMIT
#define WaitForCurrentTransmit() while(g_transmitCurrentBuffer) Timer_StaticDelayForUs(1)

uint8_t* g_transmitCurrentBuffer = NULL;
uint8_t g_transmitCurrentSize = 0;
uint8_t g_transmitCurrentIndex = 0;
bool g_bNeedTransmitCR = false;
#endif

// ////////////////////////

#if UART_ASYNC_TRANSMIT
void TransmitCurrentByte(void);
#endif
void ReceiveByte(void);

// ////////////////////////

ISR(USART_RXC_vect)
{
    ReceiveByte();
}

ISR(USART_TXC_vect)
{
    TransmitCurrentByte();
}

#if UART_ASYNC_TRANSMIT

void TransmitCurrentByte(void)
{
    if(!g_transmitCurrentBuffer || g_transmitCurrentSize == 0)
    {
        return;
    }

    if(g_bNeedTransmitCR)
    {
        g_bNeedTransmitCR = false;
        RegOverwrite(UDR, '\r');
        return; // Can safely return here because we'll have a new interrupt when this write is finished.
    }

    if(g_transmitCurrentIndex >= g_transmitCurrentSize) // We have already sent the last byte.
    {
        g_transmitCurrentBuffer = NULL;
        g_transmitCurrentSize = 0;
        g_transmitCurrentIndex = 0;

        return;
    }

    const uint8_t byte = g_transmitCurrentBuffer[g_transmitCurrentIndex];
    ++g_transmitCurrentIndex;
    g_bNeedTransmitCR = byte == '\n';

    RegOverwrite(UDR, byte);
}

#endif

void ReceiveByte(void)
{
    // TODO: Synchronize with Receive func.
    g_receiveBuffer[g_receiveBufferIndex] = RegRead(UDR);

    if(g_receiveBufferIndex == UART_RECEIVE_BUFFER_SIZE - 1)
    {
        g_receiveBufferIndex = 0;
    }
    else
    {
        ++g_receiveBufferIndex;
    }  
}

// ////////////////////////

void Uart_Init(void)
{
    g_printfBuffer[0] = 0;

    RegWrite16(UBRRL, UBRRH, UART_BAUDRATE);                    // Set baud rate.
    RegWrite(UCSRB, (1<<TXEN) | (1<<RXEN));                     // Enable receiver and transmitter.
    RegWrite(UCSRC, (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));      // 8bit data format, all other settings go as default.

    BitEnable(UCSRB, RXCIE);
#if UART_ASYNC_TRANSMIT
    BitEnable(UCSRB, TXCIE);
#endif
}

void Uart_TransmitNow(uint8_t byte)
{
    BitWait(UCSRA, UDRE, 0);            // Wait while register is free.
    RegOverwrite(UDR, byte);

    // Support for endline char carriage return.
    if(byte == '\n')
    {
        Uart_TransmitNow('\r');
    }
}

void Uart_TransmitData(uint8_t* buffer, uint8_t size)
{
#if UART_ASYNC_TRANSMIT

    if(!buffer || size == 0)
    {
        return;
    }

    // If there is any transmit in progress, we have to wait here until it is finished.
    WaitForCurrentTransmit();

    g_transmitCurrentBuffer = buffer;
    g_transmitCurrentSize = size;
    g_transmitCurrentIndex = 0;

    // Begin transmission by sending one byte now.
    TransmitCurrentByte();

#else

    for(uint8_t i = 0; i < size; ++i)
    {
        Uart_TransmitNow(buffer[i]);
    }

#endif
}

void Uart_Printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    WaitForCurrentTransmit();

    vsnprintf(g_printfBuffer, UART_PRINTF_BUFFER_SIZE, format, args);

    const uint8_t printBufferLength = strnlen(g_printfBuffer, UART_PRINTF_BUFFER_SIZE - 1);

    Uart_TransmitData((uint8_t*)g_printfBuffer, printBufferLength);

    va_end(args);
}

bool Uart_ReceiveData(uint8_t* buffer, uint8_t bufferSize, uint8_t* outBytesRead)
{
    // TODO: Synchronize with ISR.
    if(g_receiveBufferIndex == 0)
    {
        return false;
    }

    // Don't compare for buffer size, because internal buffer size is UINT8_MAX.
    memcpy(buffer, g_receiveBuffer, g_receiveBufferIndex + 1);
    *outBytesRead = g_receiveBufferIndex + 1;
    g_receiveBufferIndex = 0;
    
    return true;
}
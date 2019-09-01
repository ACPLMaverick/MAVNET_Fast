#pragma once

#include "defs.h"
#include "uart_config.h"

void Uart_Init(void);
void Uart_TransmitNow(uint8_t byte);
void Uart_TransmitData(uint8_t* buffer, uint8_t size);
void Uart_Printf(const char* format, ...);

#if UART_USE_RECEIVE
bool Uart_ReceiveData(uint8_t* buffer, uint8_t bufferSize, uint8_t* outBytesRead);
#endif

// According to the internets, strlen should be optimized out to the literal for hard-coded strings.
#define Uart_Print(text) Uart_TransmitData((uint8_t*)(text), strlen(text))
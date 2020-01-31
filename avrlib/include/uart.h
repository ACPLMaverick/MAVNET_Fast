#pragma once

#include "defs.h"
#include "uart_config.h"

void Lib_Uart_Init(void);
void Lib_Uart_TransmitNow(uint8_t byte);
void Lib_Uart_TransmitData(uint8_t* buffer, uint8_t size);
void Lib_Uart_Printf(const char* format, ...);

#if LIB_UART_USE_RECEIVE
bool Lib_Uart_ReceiveData(uint8_t* buffer, uint8_t bufferSize, uint8_t* outBytesRead);
#endif

// According to the internets, strlen should be optimized out to the literal for hard-coded strings.
#define Lib_Uart_Print(text) Lib_Uart_TransmitData((uint8_t*)(text), strlen(text))
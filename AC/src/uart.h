#pragma once

#include "defs.h"

void Uart_Init(void);
void Uart_SendNow(uint8_t byte);
void Uart_SendData(uint8_t* buffer, uint16_t size);
void Uart_Printf(const char* format, ...);

#define Uart_Print(text) Uart_SendData((uint8_t*)(text), strlen(text))
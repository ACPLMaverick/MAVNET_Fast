#pragma once
// Global definition file that should be included in every .c file.

// Some helpers
#define bool unsigned char
#define true 1
#define false 0

#define BitEnable(register, offset) (register) |= (1U << (offset))
#define BitDisable(register, offset) (register) &= ~(1U << (offset))
#define BitToggle(register, offset) (register) ^= (1U << (offset))
#define BitRead(register, offset) ((register >> (offset)) & 1U)

#define RegWrite(register, value) ((register) |= (value))
#define RegWriteHalfByte(register, firstBit, value) ((register) = ((register) & 0xff & ~(0x0f << (firstBit))) | (((value) & 0x0f) << (firstBit)))
#define RegClear(register) RegWrite(register, 0)
#define RegOverwrite(register, value) ((register) = (value))
#define RegRead(register) (register)
#define RegReadRange(register, bitFrom, bitTo) ((RegRead(register)) & ((0xFF << bitFrom) & (0xFF >> (8 - (bitTo)))))

#define BitWait(register, offset, waitValue) while (((register) & (1U << (offset))) == (waitValue))

#define RegWrite16(registerL, registerH, value)         \
{                                                       \
    (registerL) = (uint8_t)(value);                     \
    (registerH) = (uint8_t)((value) >> 8);              \
}   
#define RegRead16(value, registerL, registerH)          \
{                                                       \
    (value) = (registerL);                              \
    (value) = (uint16_t)(registerH) << 8;               \
}  
// //////////////

#define __debugbreak() while(1) /*spin*/;

// Headers
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <string.h>
// //////////////
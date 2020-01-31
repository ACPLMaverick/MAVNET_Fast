#pragma once
// Global definition file that should be included in every .c file.

// Some helpers
#define bool unsigned char
#define true 1
#define false 0

#define Lib_BitEnable(register, offset) (register) |= (1U << (offset))
#define Lib_BitDisable(register, offset) (register) &= ~(1U << (offset))
#define Lib_BitToggle(register, offset) (register) ^= (1U << (offset))
#define Lib_BitRead(register, offset) ((register >> (offset)) & 1U)

#define Lib_RegWrite(register, value) ((register) = (value))
#define Lib_RegWriteHalfByte(register, firstBit, value) ((register) = ((register) & 0xff & ~(0x0f << (firstBit))) | (((value) & 0x0f) << (firstBit)))
#define Lib_RegClear(register) Lib_RegWrite(register, 0)
#define Lib_RegMerge(register, value) ((register) |= (value))
#define Lib_RegRead(register) (register)
#define Lib_RegReadRange(register, bitFrom, bitTo) ((Lib_RegRead(register)) & ((0xFF << bitFrom) & (0xFF >> (8 - (bitTo)))))

#define Lib_BitWait(register, offset, waitValue) while (((register) & (1U << (offset))) == (waitValue))

#define Lib_RegWrite16(registerL, registerH, value)     \
{                                                       \
    (registerL) = (uint8_t)(value);                     \
    (registerH) = (uint8_t)((value) >> 8);              \
}   
#define Lib_RegRead16(value, registerL, registerH)      \
{                                                       \
    (value) = (registerL);                              \
    (value) = (uint16_t)(registerH) << 8;               \
}  
// //////////////

#if LIB_ENABLE_ASSERTS
#define __debugbreak() /*TODO Light Red Light Of Death*/ while(1) /*spin*/;
#define assert(value) { if((value) == false) { __debugbreak(); }}
#else
#define __debugbreak() /*Nothing*/
#define assert(value) /*Nothing*/
#endif

// Headers
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <string.h>
// //////////////
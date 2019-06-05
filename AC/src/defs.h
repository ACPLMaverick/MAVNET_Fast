#pragma once
// Global definition file that should be included in every .c file.

// Some helpers
#define bool unsigned char
#define true 1
#define false 0

#define BitEnable(register, offset) (register) |= (1U << (offset))
#define BitDisable(register, offset) (register) &= ~(1U << (offset))
#define BitToggle(register, offset) (register) ^= (1U << (offset))
#define BitRead(register, offset) ((register >> offset) & 1U)

#define Uint16Write(registerL, registerH, value)        \
{                                                       \
    (registerL) = (uint8_t)(value);                     \
    (registerH) = (uint8_t)((value) >> 8);              \
}   
#define Uint16Read(value, registerL, registerH)         \
{                                                       \
    (value) = (registerL);                              \
    (value) = (uint16_t)(registerH) << 8;               \
}  
// //////////////

// Headers
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <string.h>
// //////////////
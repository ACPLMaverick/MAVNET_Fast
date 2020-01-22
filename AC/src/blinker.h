#pragma once

#include "defs.h"
#include "blinker_config.h"

#define BLINKER_MAX_NUM_PINS 8

void Blinker_Init(void);

#define Blinker_CreateFuncs(index)                                             \
inline void Blinker_On_##index(void) { BitEnable(BLINKER_PORT, index); }       \
inline void Blinker_Off_##index(void) { BitDisable(BLINKER_PORT, index); }     \
inline void Blinker_Toggle_##index(void) { BitToggle(BLINKER_PORT, index); }

#if BLINKER_NUM_PINS > BLINKER_MAX_NUM_PINS
#error "LIB: Blinker: Number of pins exceeded."
#else

#if BLINKER_NUM_PINS > 0
Blinker_CreateFuncs(0)
#endif
#if BLINKER_NUM_PINS > 1
Blinker_CreateFuncs(1)
#endif
#if BLINKER_NUM_PINS > 2
Blinker_CreateFuncs(2)
#endif
#if BLINKER_NUM_PINS > 3
Blinker_CreateFuncs(3)
#endif
#if BLINKER_NUM_PINS > 4
Blinker_CreateFuncs(4)
#endif
#if BLINKER_NUM_PINS > 5
Blinker_CreateFuncs(5)
#endif
#if BLINKER_NUM_PINS > 6
Blinker_CreateFuncs(6)
#endif
#if BLINKER_NUM_PINS > 7
Blinker_CreateFuncs(7)
#endif

#endif
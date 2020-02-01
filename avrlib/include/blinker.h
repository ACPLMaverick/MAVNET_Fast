#pragma once

#if LIB_USE_BLINKER

#include "defs.h"
#include "blinker_config.h"

#define LIB_BLINKER_MAX_NUM_PINS 8

void Lib_Blinker_Init(void);

#define Lib_Blinker_CreateFuncs(index)                                             \
inline void Lib_Blinker_On_##index(void) { Lib_BitEnable(LIB_BLINKER_PORT, index); }       \
inline void Lib_Blinker_Off_##index(void) { Lib_BitDisable(LIB_BLINKER_PORT, index); }     \
inline void Lib_Blinker_Toggle_##index(void) { Lib_BitToggle(LIB_BLINKER_PORT, index); }

#if LIB_BLINKER_NUM_PINS > LIB_BLINKER_MAX_NUM_PINS
#error "LIB: Blinker: Number of pins exceeded."
#else

#if LIB_BLINKER_NUM_PINS > 0
Lib_Blinker_CreateFuncs(0)
#endif
#if LIB_BLINKER_NUM_PINS > 1
Lib_Blinker_CreateFuncs(1)
#endif
#if LIB_BLINKER_NUM_PINS > 2
Lib_Blinker_CreateFuncs(2)
#endif
#if LIB_BLINKER_NUM_PINS > 3
Lib_Blinker_CreateFuncs(3)
#endif
#if LIB_BLINKER_NUM_PINS > 4
Lib_Blinker_CreateFuncs(4)
#endif
#if LIB_BLINKER_NUM_PINS > 5
Lib_Blinker_CreateFuncs(5)
#endif
#if LIB_BLINKER_NUM_PINS > 6
Lib_Blinker_CreateFuncs(6)
#endif
#if LIB_BLINKER_NUM_PINS > 7
Lib_Blinker_CreateFuncs(7)
#endif

#endif

#endif
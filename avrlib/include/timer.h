#pragma once

#if LIB_USE_TIMER

#include "defs.h"

#include <util/delay.h>

#define LIB_TIMER_CALL_NUM_PERSISTENT               0
#define LIB_TIMER_OVERFLOW_VALUE_8                  ((uint32_t)UINT8_MAX + 1ULL)
#define LIB_TIMER_OVERFLOW_VALUE_16                 ((uint32_t)UINT16_MAX + 1ULL)
#define LIB_TIMER_TICK_MAX_VALUE_8                  ((uint32_t)UINT8_MAX * (uint32_t)UINT16_MAX)
#define LIB_TIMER_TICK_MAX_VALUE_16                 ((uint32_t)UINT16_MAX * (uint32_t)UINT16_MAX)
#define Lib_Timer_TickCountFromMs(time)             ((((time) * F_CPU) / 1000ULL) - 1)
#define Lib_Timer_TickCountFromUs(time)             ((((time) * F_CPU) / 1000000ULL) - 1)
#define Lib_Timer_MsFromTickCount(ticks)            ((uint16_t)(( (float)(ticks) / (float)F_CPU) * 1000.0f))
#define Lib_Timer_UsFromTickCount(ticks)            ((uint32_t)(( (float)(ticks) / (float)F_CPU) * 1000000.0f))
#define Lib_Timer_OverflowNumFromTicks_8(ticks)     ((ticks) / LIB_TIMER_OVERFLOW_VALUE_8)
#define Lib_Timer_RemainderFromTicks_8(ticks)       ((ticks) % LIB_TIMER_OVERFLOW_VALUE_8)
#define Lib_Timer_OverflowNumFromTicks_16(ticks)    ((ticks) / LIB_TIMER_OVERFLOW_VALUE_16)
#define Lib_Timer_RemainderFromTicks_16(ticks)      ((ticks) % LIB_TIMER_OVERFLOW_VALUE_16)

typedef void (*Lib_Timer_CallbackFunc)(void*);
typedef void* Lib_Timer_CallbackParam;


void Lib_Timer_Init(void);

void Lib_Timer_ScheduleCallback_1_Ext(uint16_t tickOverflows, uint16_t tickRemainder, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum);
void Lib_Timer_RemoveCallback_1(void);
#define Lib_Timer_ScheduleCallback_1(ticks, func, param, callNum) Lib_Timer_ScheduleCallback_1_Ext(Lib_Timer_OverflowNumFromTicks_16(ticks), Lib_Timer_RemainderFromTicks_16(ticks), func, param, callNum)
#define Lib_Timer_ScheduleCallbackMs_1(ms, func, param, callNum) Lib_Timer_ScheduleCallback_1(Lib_Timer_TickCountFromMs(ms), func, param, callNum)
#define Lib_Timer_ScheduleCallbackUs_1(us, func, param, callNum) Lib_Timer_ScheduleCallback_1(Lib_Timer_TickCountFromUs(us), func, param, callNum)

void Lib_Timer_ScheduleCallback_2_Ext(uint16_t tickOverflows, uint16_t tickRemainder, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum);
void Lib_Timer_RemoveCallback_2(void);
#define Lib_Timer_ScheduleCallback_2(ticks, func, param, callNum) Lib_Timer_ScheduleCallback_2_Ext(Lib_Timer_OverflowNumFromTicks_8(ticks), Lib_Timer_RemainderFromTicks_8(ticks), func, param, callNum)
#define Lib_Timer_ScheduleCallbackMs_2(ms, func, param, callNum) Lib_Timer_ScheduleCallback_2(Lib_Timer_TickCountFromMs(ms), func, param, callNum)
#define Lib_Timer_ScheduleCallbackUs_2(us, func, param, callNum) Lib_Timer_ScheduleCallback_2(Lib_Timer_TickCountFromUs(us), func, param, callNum)

void Lib_Timer_SetDurationMarker(void);
uint32_t Lib_Timer_GetDurationTicks(void);
inline uint16_t Timer_GetDurationMs(void) { return Lib_Timer_MsFromTickCount(Lib_Timer_GetDurationTicks()); }
inline uint32_t Timer_GetDurationUs(void) { return Lib_Timer_UsFromTickCount(Lib_Timer_GetDurationTicks()); }

#define Lib_Timer_SleepMs(value) _delay_ms(value)
#define Lib_Timer_SleepUs(value) _delay_us(value)
void Lib_Timer_SleepVarMs(uint16_t delay);
void Lib_Timer_SleepVarUs(uint16_t delay);

#endif
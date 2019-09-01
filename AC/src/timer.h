#pragma once

#include "defs.h"

#include <util/delay.h>

#define TIMER_CALL_NUM_PERSISTENT               0
#define TIMER_OVERFLOW_VALUE_8                  ((uint32_t)UINT8_MAX + 1ULL)
#define TIMER_OVERFLOW_VALUE_16                 ((uint32_t)UINT16_MAX + 1ULL)
#define TIMER_TICK_MAX_VALUE_8                  ((uint32_t)UINT8_MAX * (uint32_t)UINT16_MAX)
#define TIMER_TICK_MAX_VALUE_16                 ((uint32_t)UINT16_MAX * (uint32_t)UINT16_MAX)
#define Timer_TickCountFromMs(time)             ((((time) * F_CPU) / 1000ULL) - 1)
#define Timer_TickCountFromUs(time)             ((((time) * F_CPU) / 1000000ULL) - 1)
#define Timer_OverflowNumFromTicks_8(ticks)     ((ticks) / TIMER_OVERFLOW_VALUE_8)
#define Timer_RemainderFromTicks_8(ticks)       ((ticks) % TIMER_OVERFLOW_VALUE_8)
#define Timer_OverflowNumFromTicks_16(ticks)    ((ticks) / TIMER_OVERFLOW_VALUE_16)
#define Timer_RemainderFromTicks_16(ticks)      ((ticks) % TIMER_OVERFLOW_VALUE_16)

typedef void (*Timer_CallbackFunc)(void*);
typedef void* Timer_CallbackParam;


void Timer_Init(void);

void Timer_ScheduleCallbackExt_1(uint16_t tickOverflows, uint16_t tickRemainder, Timer_CallbackFunc func, Timer_CallbackParam param, uint8_t callNum);
void Timer_RemoveCallback_1(void);
#define Timer_ScheduleCallback_1(ticks, func, param, callNum) Timer_ScheduleCallbackExt_1(Timer_OverflowNumFromTicks_16(ticks), Timer_RemainderFromTicks_16(ticks), func, param, callNum)
#define Timer_ScheduleCallbackMs_1(ms, func, param, callNum) Timer_ScheduleCallback_1(Timer_TickCountFromMs(ms), func, param, callNum)
#define Timer_ScheduleCallbackUs_1(us, func, param, callNum) Timer_ScheduleCallback_1(Timer_TickCountFromUs(us), func, param, callNum)

void Timer_ScheduleCallbackExt_2(uint16_t tickOverflows, uint16_t tickRemainder, Timer_CallbackFunc func, Timer_CallbackParam param, uint8_t callNum);
void Timer_RemoveCallback_2(void);
#define Timer_ScheduleCallback_2(ticks, func, param, callNum) Timer_ScheduleCallbackExt_2(Timer_OverflowNumFromTicks_8(ticks), Timer_RemainderFromTicks_8(ticks), func, param, callNum)
#define Timer_ScheduleCallbackMs_2(ms, func, param, callNum) Timer_ScheduleCallback_2(Timer_TickCountFromMs(ms), func, param, callNum)
#define Timer_ScheduleCallbackUs_2(us, func, param, callNum) Timer_ScheduleCallback_2(Timer_TickCountFromUs(us), func, param, callNum)

// TODO Timer_SetMarker
// TODO Timer_GetDurationTicks
// TODO Timer_GetDurationMs
// TODO Timer_GetDurationUs

// TODO Timer_WhileMs
// TODO Timer_WhileUs

#define Timer_SleepMs(value) _delay_ms(value)
#define Timer_SleepUs(value) _delay_us(value)
void Timer_SleepVarMs(uint16_t delay);
void Timer_SleepVarUs(uint16_t delay);
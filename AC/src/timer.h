#pragma once

#include "defs.h"

#include <util/delay.h>

typedef void (*CallbackFunc)(void*);

typedef enum TimerUnits
{
    TimerUnits_kMiliseconds,
    TimerUnits_kMicroseconds,

    TimerUnits_kNum

} TimerUnits;

typedef struct TimerCallbackInfo
{
    uint16_t m_time;
    TimerUnits m_units;
    CallbackFunc m_func;
    void* m_param;
    bool m_persistent;

} TimerCallbackInfo;

void Timer_Init(void);

bool Timer_ScheduleCallback(const TimerCallbackInfo* callbackInfo);
bool Timer_RemoveCalback(CallbackFunc func);

#define Timer_StaticDelayForMs(value) _delay_ms(value)
#define Timer_StaticDelayForUs(value) _delay_us(value)
void Timer_DelayForMs(uint16_t delay);
void Timer_DelayForUs(uint16_t delay);
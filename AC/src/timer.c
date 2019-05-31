#include "timer.h"

typedef enum TimerType
{
    TimerType_k8_0,
    TimerType_k8_2,
    TimerType_k16_1,

    TimerType_kNum

} TimerType;

// This struct is gonna possess some additional timer data.
typedef struct TimerCallback
{
    uint16_t m_tickOverflowsToGo;
    uint16_t m_tickRemainder;
    CallbackFunc m_func;
    void* m_param;
    bool m_persistent;

} TimerCallback;


static TimerCallbackInfo g_callbacks[(uint8_t)TimerType_kNum];


uint32_t GetTickCountFromMs(uint16_t delay)
{
    return ((delay * F_CPU) / 1000) - 1;
}

uint32_t GetTickCountFromUs(uint16_t delay)
{
    return ((delay * F_CPU) / 1000000) - 1;
}

typedef uint32_t (*GetTickCountFunc)(uint16_t);
static const GetTickCountFunc g_getTickCountFuncs[(uint8_t)TimerUnits_kNum] = 
{
    GetTickCountFromMs,
    GetTickCountFromUs
};

bool IsTimerOccupied(TimerType timerType)
{
    return g_callbacks[(uint8_t)timerType].m_func != NULL;
}

bool PickTimer(const TimerCallbackInfo* callbackInfo, TimerType* outTimerType)
{
    TimerType firstFreeTimer = TimerType_kNum;
    for(uint8_t i = 0; i < (uint8_t)TimerType_kNum; ++i)
    {
        if(!IsTimerOccupied((TimerType)i))
        {
            firstFreeTimer = (TimerType)i;
            break;
        }
    }

    if(firstFreeTimer == TimerType_kNum)
    {
        // No free timers.
        return false;
    }

    uint32_t tickCount = g_getTickCountFuncs[(uint8_t)callbackInfo->m_units](callbackInfo->m_time);
    return false;
}

// This function assumes that callback has already been assigned.
void InitTimer(TimerType timerType)
{
    switch (timerType)
    {
    case TimerType_k8_0:
        {

        }
        break;
    case TimerType_k8_2:
        {

        }
        break;
    case TimerType_k16_1:
        {
            
        }
        break;
    
    default:
        break;
    }
}

void ClearTimer(TimerType timerType)
{
    switch (timerType)
    {
    case TimerType_k8_0:
        {

        }
        break;
    case TimerType_k8_2:
        {

        }
        break;
    case TimerType_k16_1:
        {
            
        }
        break;
    
    default:
        break;
    }
}



void Timer_Init(void)
{
    // TODO Check if this is correct in C.
    memset(g_callbacks, 0, sizeof(g_callbacks));
}

bool Timer_ScheduleCallback(const TimerCallbackInfo* callbackInfo)
{
    if(!callbackInfo || !callbackInfo->m_func)
    {
        return false;
    }

    if(callbackInfo->m_time == 0)
    {
        callbackInfo->m_func(callbackInfo->m_param);
        return true;
    }

    TimerType timerType;
    if(!PickTimer(callbackInfo, &timerType))
    {
        return false;
    }

    InitTimer(timerType);

    return true;
}

bool Timer_RemoveCalback(CallbackFunc func)
{
    for(uint8_t i = 0; i < (uint8_t)TimerType_kNum; ++i)
    {
        if(func == g_callbacks[i].m_func)
        {
            cli();
            memset(&g_callbacks[i], 0, sizeof(g_callbacks[i]));
            sei();

            ClearTimer((TimerType)i);

            return true;
        }
    }

    return false;
}

void Timer_DelayForMs(uint16_t delay)
{
    for(uint16_t i = 0; i < delay; ++i)
    {
        _delay_ms(1);
    }
}

void Timer_DelayForUs(uint16_t delay)
{
    for(uint16_t i = 0; i < delay; ++i)
    {
        _delay_us(1);
    }
}
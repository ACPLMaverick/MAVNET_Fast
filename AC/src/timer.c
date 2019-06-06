#include "timer.h"

#include "blinker.h"

#define MAX_TICKS_8B 0
#define MAX_TICKS_8B_PRESCALER_8 0
#define MAX_TICKS_8B_PRESCALER_64 0
#define MAX_TICKS_8B_PRESCALER_256 0
#define MAX_TICKS_8B_PRESCALER_1024 0

typedef enum TimerType
{
    TimerType_k8,       // TIMER2
    TimerType_k16,      // TIMER1
                        // TIMER0 is reserved for Marker setting purpose.
    TimerType_kNum

} TimerType;

// This struct is gonna possess some additional timer data.
typedef struct TimerCallback
{
    uint16_t m_baseTickOverflowsToGo;
    uint16_t m_tickOverflowsToGo;
    uint16_t m_tickRemainder;
    CallbackFunc m_func;
    void* m_param;
    bool m_persistent;

} TimerCallback;



void ProcessISROverflow(TimerType timerType);
void ProcessISRCompare(TimerType timerType);
void PerformCall(TimerType timerType);
inline uint32_t GetTickCountFromMs(uint16_t delay);
inline uint32_t GetTickCountFromUs(uint16_t delay);
inline uint32_t GetTickCount(uint16_t time, TimerUnits units);
void InitTimer0(void);
void InitTimer1(void);
void InitTimer2(void);
void ClearTimer0(void);
void ClearTimer1(void);
void ClearTimer2(void);
inline void InitTimer(TimerType timerType);
inline void ClearTimer(TimerType timerType);
inline bool IsTimerOccupied(TimerType timerType);
bool PickTimer(uint32_t ticks, TimerType* outTimerType, uint16_t* outTickOverflowsToGo, uint16_t* outTickRemainder);
void InitTimerAndData(uint16_t tickOverflowsToGo, uint16_t tickRemainder, TimerType timerType, const TimerCallbackInfo* callbackInfo);
inline void ClearData(TimerType timerType);



static TimerCallback g_callbacks[(uint8_t)TimerType_kNum];

typedef uint32_t (*GetTickCountFunc)(uint16_t);
static const GetTickCountFunc g_getTickCountFuncs[(uint8_t)TimerUnits_kNum] = 
{
    GetTickCountFromMs,
    GetTickCountFromUs
};

typedef void (*InitTimerFunc)(void);
static const InitTimerFunc g_initTimerFuncs[(uint8_t)TimerType_kNum] = 
{
    InitTimer2,
    InitTimer1
};

typedef void (*ClearTimerFunc)(void);
static const ClearTimerFunc g_clearTimerFuncs[(uint8_t)TimerType_kNum] = 
{
    ClearTimer2,
    ClearTimer1
};

static const uint32_t g_timerOverflowValues[(uint8_t)TimerType_kNum] = 
{
    UINT8_MAX + 1ULL,
    UINT16_MAX + 1ULL
};

static const uint32_t g_timerTickMaxValues[(uint8_t)TimerType_kNum] = 
{
    (uint32_t)UINT8_MAX * (uint32_t)UINT16_MAX,
    (uint32_t)UINT16_MAX * (uint32_t)UINT16_MAX
};



ISR(TIMER0_OVF_vect)
{
    // TODO Markers
}

ISR(TIMER1_OVF_vect)
{
    ProcessISROverflow(TimerType_k16);
}

ISR(TIMER2_OVF_vect)
{
    ProcessISROverflow(TimerType_k8);
}

ISR(TIMER1_COMPA_vect)
{
    ProcessISRCompare(TimerType_k16);
}

ISR(TIMER2_COMP_vect)
{
    ProcessISRCompare(TimerType_k8);
}


void ProcessISROverflow(TimerType timerType)
{
    TimerCallback* callbackData = &g_callbacks[(uint8_t)timerType];
    if(callbackData->m_tickOverflowsToGo > 0)
    {
        --callbackData->m_tickOverflowsToGo;
    }
    else if(callbackData->m_tickRemainder == 0)
    {
        PerformCall(timerType);
    }
    else
    {
        // Schedule for compare ISR.
        if(timerType == TimerType_k16)
        {
            // Timer1.
            BitEnable(TCCR1B, WGM12);       // Enable CTC, TOP in OCR1A
            Uint16Write(OCR1AL, OCR1AH, callbackData->m_tickRemainder); // Store value.
            BitEnable(TIMSK, OCIE1A);       // Enable interrupt on CTC value.
        }
        else if(timerType == TimerType_k8)
        {
            // Timer2.
            BitEnable(TCCR2, WGM21);
            OCR2 = (uint8_t)callbackData->m_tickRemainder;
            BitEnable(TIMSK, OCIE2);
        }
    }
    
}

void ProcessISRCompare(TimerType timerType)
{
    // Clear CTC.
    if(timerType == TimerType_k16)
    {
        // Timer1.
        BitDisable(TCCR1B, WGM12);
        BitDisable(TIMSK, OCIE1A);
    }
    else if(timerType == TimerType_k8)
    {
        // Timer2.
        BitDisable(TCCR2, WGM21);
        BitDisable(TIMSK, OCIE2);
    }

    // Compare is always the last ISR in timeline.
    PerformCall(timerType);
}

void PerformCall(TimerType timerType)
{
    TimerCallback* callbackData = &g_callbacks[(uint8_t)timerType];
    if(!callbackData->m_persistent)
    {
        CallbackFunc func = callbackData->m_func;
        void* param = callbackData->m_param;

        ClearData(timerType);
        ClearTimer(timerType);

        func(param);
    }
    else
    {
        callbackData->m_tickOverflowsToGo = callbackData->m_baseTickOverflowsToGo;
        callbackData->m_func(callbackData->m_param);
    }
}

uint32_t GetTickCountFromMs(uint16_t delay)
{
    return ((delay * F_CPU) / 1000ULL) - 1;
}

uint32_t GetTickCountFromUs(uint16_t delay)
{
    return ((delay * F_CPU) / 1000000ULL) - 1;
}

uint32_t GetTickCount(uint16_t time, TimerUnits units)
{
    return g_getTickCountFuncs[(uint8_t)units](time);
}

void InitTimer0(void)
{
    BitEnable(TCCR0, CS00);
    TCNT0 = 0;
    BitEnable(TIMSK, TOIE0);
}

void InitTimer1(void)
{
    BitEnable(TCCR1B, CS10);    // Enable this timer with no prescaler.
    TCNT1 = 0;                  // Set counter to 0.
    BitEnable(TIMSK, TOIE1);    // Enable overflow interrupt.
}

void InitTimer2(void)
{
    BitEnable(TCCR2, CS20);
    TCNT2 = 0;
    BitEnable(TIMSK, TOIE2);
}

void ClearTimer0(void)
{
    TCCR0 = 0;  // Clear this register, i.e. disable the timer.
    TCNT0 = 0;
    BitDisable(TIMSK, TOIE0);
}

void ClearTimer1(void)
{
    TCCR1B = 0;
    TCNT1 = 0;
    BitDisable(TIMSK, TOIE1);
}

void ClearTimer2(void)
{
    TCCR2 = 0;
    TCNT2 = 0;
    BitDisable(TIMSK, TOIE2);
}

// This function assumes that callback has already been assigned.
void InitTimer(TimerType timerType)
{
    g_initTimerFuncs[(uint8_t)timerType]();
}

void ClearTimer(TimerType timerType)
{
    g_clearTimerFuncs[(uint8_t)timerType]();
}

bool IsTimerOccupied(TimerType timerType)
{
    return g_callbacks[(uint8_t)timerType].m_func != NULL;
}

bool PickTimer(uint32_t ticks, TimerType* outTimerType, uint16_t* outTickOverflowsToGo, uint16_t* outTickRemainder)
{
    TimerType freeTimer = TimerType_kNum;
    for(uint8_t i = 0; i < (uint8_t)TimerType_kNum; ++i)
    {
        if(!IsTimerOccupied((TimerType)i) && ticks <= g_timerTickMaxValues[i])
        {
            freeTimer = (TimerType)i;
            break;
        }
    }

    if(freeTimer == TimerType_kNum)
    {
        // No free timers or tick value too big.
        return false;
    }

    // Let's not use prescaler for now...
    *outTickOverflowsToGo = (uint16_t)(ticks / g_timerOverflowValues[(uint8_t)freeTimer]);
    *outTickRemainder = (uint16_t)(ticks % g_timerOverflowValues[(uint8_t)freeTimer]);
    *outTimerType = freeTimer;

    return true;
}

void InitTimerAndData(uint16_t tickOverflowsToGo, uint16_t tickRemainder, TimerType timerType, const TimerCallbackInfo* callbackInfo)
{
    cli();

    g_callbacks[(uint8_t)timerType].m_baseTickOverflowsToGo = tickOverflowsToGo;
    g_callbacks[(uint8_t)timerType].m_tickOverflowsToGo = tickOverflowsToGo;
    g_callbacks[(uint8_t)timerType].m_tickRemainder = tickRemainder;
    g_callbacks[(uint8_t)timerType].m_func = callbackInfo->m_func;
    g_callbacks[(uint8_t)timerType].m_param = callbackInfo->m_param;
    g_callbacks[(uint8_t)timerType].m_persistent = callbackInfo->m_persistent;

    InitTimer(timerType);

    sei();
}

void ClearData(TimerType timerType)
{
    memset(&g_callbacks[(uint8_t)timerType], 0, sizeof(TimerCallback));
}


// //////////////////////////////////


void Timer_Init(void)
{
    // TODO Check if this is correct in C.
    memset(g_callbacks, 0, sizeof(g_callbacks));
}

bool Timer_ScheduleCallback(uint16_t time, TimerUnits units, const TimerCallbackInfo* callbackInfo)
{
    const uint32_t ticks = GetTickCount(time, units);

    // ++test code
    /*
    static const TimerType tt = TimerType_k8;
    g_callbacks[(uint8_t)tt].m_baseTickOverflowsToGo = ticks / g_timerOverflowValues[(uint8_t)tt];
    g_callbacks[(uint8_t)tt].m_tickOverflowsToGo = g_callbacks[(uint8_t)tt].m_baseTickOverflowsToGo;
    g_callbacks[(uint8_t)tt].m_tickRemainder = ticks % g_timerOverflowValues[(uint8_t)tt];
    g_callbacks[(uint8_t)tt].m_persistent = true;

    g_callbacks[(uint8_t)tt].m_func = callbackInfo->m_func;
    InitTimer2();

    return true;
    */
    // --test code

    return Timer_ScheduleCallbackTicks(ticks, callbackInfo);
}

bool Timer_ScheduleCallbackTicks(uint32_t ticks, const TimerCallbackInfo* callbackInfo)
{
    if(!callbackInfo || !callbackInfo->m_func)
    {
        return false;
    }

    if(ticks == 0)
    {
        callbackInfo->m_func(callbackInfo->m_param);
        return true;
    }

    TimerType timerType;
    uint16_t tickOverflowsToGo;
    uint16_t tickRemainder;
    if(!PickTimer(ticks, &timerType, &tickOverflowsToGo, &tickRemainder))
    {
        return false;
    }

    InitTimerAndData(tickOverflowsToGo, tickRemainder, timerType, callbackInfo);

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
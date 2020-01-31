#include "timer.h"

#include "blinker.h"

#define MAX_TICKS_8B 0
#define MAX_TICKS_8B_PRESCALER_8 0
#define MAX_TICKS_8B_PRESCALER_64 0
#define MAX_TICKS_8B_PRESCALER_256 0
#define MAX_TICKS_8B_PRESCALER_1024 0

// TODO only local interrupts
#define HoldInterruptsTimer0()      cli()
#define ResumeInterruptsTimer0()    sei()
#define HoldInterruptsTimer1()      cli()
#define ResumeInterruptsTimer1()    sei()
#define HoldInterruptsTimer2()      cli()
#define ResumeInterruptsTimer2()    sei()
// ~TODO

typedef enum TimerType
{
    // TIMER0 is reserved for Marker setting purpose.
    TimerType_k16_1,    // TIMER1
    TimerType_k8_2,     // TIMER2

    TimerType_kNum

} TimerType;

// This struct is gonna possess some additional timer data.
typedef struct TimerCallback
{
    uint16_t m_baseTickOverflowsToGo;
    uint16_t m_tickOverflowsToGo;
    uint16_t m_tickRemainder;
    Lib_Timer_CallbackFunc m_func;
    void* m_param;
    uint8_t m_callNum;

} TimerCallback;



void ProcessISROverflow(TimerType timerType);
void ProcessISRCompare(TimerType timerType);
void PerformCall(TimerType timerType);
inline void InitTimer0(void);
inline void InitTimer1(void);
inline void InitTimer2(void);
inline void ClearTimer0(void);
inline void ClearTimer1(void);
inline void ClearTimer2(void);
static inline void InitData(uint16_t tickOverflowsToGo, uint16_t tickRemainder, TimerType timerType, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum);
static inline void ClearData(TimerType timerType);



static TimerCallback g_callbacks[(uint8_t)TimerType_kNum];
static uint32_t g_durationOverflows;


ISR(TIMER0_OVF_vect)
{
    ++g_durationOverflows;
}

ISR(TIMER1_OVF_vect)
{
    ProcessISROverflow(TimerType_k16_1);
}

ISR(TIMER2_OVF_vect)
{
    ProcessISROverflow(TimerType_k8_2);
}

ISR(TIMER1_COMPA_vect)
{
    ProcessISRCompare(TimerType_k16_1);
}

ISR(TIMER2_COMP_vect)
{
    ProcessISRCompare(TimerType_k8_2);
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
        if(timerType == TimerType_k16_1)
        {
            // Timer1.
            Lib_BitEnable(TCCR1B, WGM12);       // Enable CTC, TOP in OCR1A
            Lib_RegWrite16(OCR1AL, OCR1AH, callbackData->m_tickRemainder); // Store value.
            Lib_BitEnable(TIMSK, OCIE1A);       // Enable interrupt on CTC value.
        }
        else if(timerType == TimerType_k8_2)
        {
            // Timer2.
            Lib_BitEnable(TCCR2, WGM21);
            OCR2 = (uint8_t)callbackData->m_tickRemainder;
            Lib_BitEnable(TIMSK, OCIE2);
        }
    }
    
}

void ProcessISRCompare(TimerType timerType)
{
    // Clear CTC.
    if(timerType == TimerType_k16_1)
    {
        // Timer1.
        Lib_BitDisable(TCCR1B, WGM12);
        Lib_BitDisable(TIMSK, OCIE1A);
    }
    else if(timerType == TimerType_k8_2)
    {
        // Timer2.
        Lib_BitDisable(TCCR2, WGM21);
        Lib_BitDisable(TIMSK, OCIE2);
    }

    // Compare is always the last ISR in timeline.
    PerformCall(timerType);
}

void PerformCall(TimerType timerType)
{
    TimerCallback* callbackData = &g_callbacks[(uint8_t)timerType];
    if(callbackData->m_callNum == LIB_TIMER_CALL_NUM_PERSISTENT + 1)    // One last call.
    {
        Lib_Timer_CallbackFunc func = callbackData->m_func;
        void* param = callbackData->m_param;

        ClearData(timerType);
        
        if(timerType == TimerType_k16_1)
        {
            ClearTimer1();
        }
        else if(timerType == TimerType_k8_2)
        {
            ClearTimer2();
        }

        func(param);
    }
    else
    {
        callbackData->m_tickOverflowsToGo = callbackData->m_baseTickOverflowsToGo;
        callbackData->m_func(callbackData->m_param);

        if(callbackData->m_callNum != LIB_TIMER_CALL_NUM_PERSISTENT)
        {
            --callbackData->m_callNum;
        }
    }
}

inline void InitTimer0(void)
{
    Lib_BitEnable(TCCR0, CS00);
    TCNT0 = 0;
    Lib_BitEnable(TIMSK, TOIE0);
}

inline void InitTimer1(void)
{
    Lib_BitEnable(TCCR1B, CS10);    // Enable this timer with no prescaler.
    TCNT1 = 0;                  // Set counter to 0.
    Lib_BitEnable(TIMSK, TOIE1);    // Enable overflow interrupt.
}

inline void InitTimer2(void)
{
    Lib_BitEnable(TCCR2, CS20);
    TCNT2 = 0;
    Lib_BitEnable(TIMSK, TOIE2);
}

inline void ClearTimer0(void)
{
    TCCR0 = 0;  // Clear this register, i.e. disable the timer.
    TCNT0 = 0;
    Lib_BitDisable(TIMSK, TOIE0);
}

inline void ClearTimer1(void)
{
    TCCR1B = 0;
    TCNT1 = 0;
    Lib_BitDisable(TIMSK, TOIE1);
}

inline void ClearTimer2(void)
{
    TCCR2 = 0;
    TCNT2 = 0;
    Lib_BitDisable(TIMSK, TOIE2);
}

static inline void InitData(uint16_t tickOverflowsToGo, uint16_t tickRemainder, TimerType timerType, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum)
{
    g_callbacks[(uint8_t)timerType].m_baseTickOverflowsToGo = tickOverflowsToGo;
    g_callbacks[(uint8_t)timerType].m_tickOverflowsToGo = tickOverflowsToGo;
    g_callbacks[(uint8_t)timerType].m_tickRemainder = tickRemainder;
    g_callbacks[(uint8_t)timerType].m_func = func;
    g_callbacks[(uint8_t)timerType].m_param = param;
    g_callbacks[(uint8_t)timerType].m_callNum = callNum;
}

static inline void ClearData(TimerType timerType)
{
    memset(&g_callbacks[(uint8_t)timerType], 0, sizeof(TimerCallback));
}


// //////////////////////////////////


void Lib_Timer_Init(void)
{
    // TODO Check if this is correct in C.
    memset(g_callbacks, 0, sizeof(g_callbacks));
    g_durationOverflows = 0;
}

void Lib_Timer_ScheduleCallback_1_Ext(uint16_t tickOverflows, uint16_t tickRemainder, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum)
{
    HoldInterruptsTimer2();
    InitData(tickOverflows, tickRemainder, TimerType_k16_1, func, param, callNum);
    InitTimer1();
    ResumeInterruptsTimer2();
}

void Lib_Timer_RemoveCallback_1(void)
{
    HoldInterruptsTimer1();
    ClearData(TimerType_k16_1);
    ClearTimer1();
    ResumeInterruptsTimer1();
}

void Lib_Timer_ScheduleCallback_2_Ext(uint16_t tickOverflows, uint16_t tickRemainder, Lib_Timer_CallbackFunc func, Lib_Timer_CallbackParam param, uint8_t callNum)
{
    HoldInterruptsTimer2();
    InitData(tickOverflows, tickRemainder, TimerType_k8_2, func, param, callNum);
    InitTimer2();
    ResumeInterruptsTimer2();
}

void Lib_Timer_RemoveCallback_2(void)
{
    HoldInterruptsTimer2();
    ClearData(TimerType_k8_2);
    ClearTimer2();
    ResumeInterruptsTimer2();
}

void Lib_Timer_SetDurationMarker(void)
{
    HoldInterruptsTimer0();
    g_durationOverflows = 0;
    InitTimer0();
    ResumeInterruptsTimer0();
}

uint32_t Lib_Timer_GetDurationTicks(void)
{
    HoldInterruptsTimer0();
    const uint8_t remainder = TCNT0;
    ClearTimer0();
    ResumeInterruptsTimer0();

    return g_durationOverflows * UINT8_MAX + remainder;
}

void Lib_Timer_SleepVarMs(uint16_t delay)
{
    for(uint16_t i = 0; i < delay; ++i)
    {
        _delay_ms(1);
    }
}

void Lib_Timer_SleepVarUs(uint16_t delay)
{
    for(uint16_t i = 0; i < delay; ++i)
    {
        _delay_us(1);
    }
}
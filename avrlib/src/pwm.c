#include "pwm.h"

#if LIB_USE_PWM

#if !defined(LIB_PWM_TIMER_1_8BIT_MODE) || LIB_PWM_TIMER_1_8BIT_MODE == 0
#define LIB_PWM_TIMER_1_10BIT_MODE 1
#else
#define LIB_PWM_TIMER_1_16BIT_MODE 0
#endif


#if LIB_PWM_USE_TIMER_1

static inline void InitPwm_1(void)
{
    // Please refer to the Table 21-5.
#if LIB_PWM_TIMER_1_8BIT_MODE
    Lib_RegWrite(TCCR1A, (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1));
    Lib_RegWrite(TCCR1B, (1 << WGM12) | (1 << CS10));
#else
    Lib_RegWrite(TCCR1A, (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1));
    Lib_RegWrite(TCCR1B, (1 << WGM12) | (1 << WGM13) | (1 << CS10));
    Lib_RegWrite(ICR1, UINT16_MAX);
#endif

    Lib_RegClearWhole(OCR1A);
    Lib_RegClearWhole(OCR1B);

    // It is mandatory to set OC1A and OC1B to output.
    Lib_BitEnable(LIB_PWM_OUTPUT_1_DDR, LIB_PWM_OUTPUT_1A_BIT);
    Lib_BitEnable(LIB_PWM_OUTPUT_1_DDR, LIB_PWM_OUTPUT_1B_BIT);
}

#if LIB_PWM_TIMER_1_8BIT_MODE
void Lib_Pwm_Set_1A(uint8_t dutyCycle)
{
    Lib_RegWrite(OCR1AL, dutyCycle);
}

uint8_t Lib_Pwm_Get_1A(void)
{
    return Lib_RegRead(OCR1AL);
}

void Lib_Pwm_Set_1B(uint8_t dutyCycle)
{
    Lib_RegWrite(OCR1BL, dutyCycle);
}

uint8_t Lib_Pwm_Get_1B(void)
{
    return Lib_RegRead(OCR1BL);
}
#else
void Lib_Pwm_Set_1A(uint16_t dutyCycle)
{
    Lib_RegWrite(OCR1A, dutyCycle);
}

uint16_t Lib_Pwm_Get_1A(void)
{
    return OCR1A;
}

void Lib_Pwm_Set_1B(uint16_t dutyCycle)
{
    Lib_RegWrite(OCR1B, dutyCycle);
}

uint16_t Lib_Pwm_Get_1B(void)
{
    return OCR1B;
}
#endif

#endif
#if LIB_PWM_USE_TIMER_2

static inline void InitPwm_2(void)
{
    // Clear whole register by overwriting.
    // Bit 6,3 – WGM01:0 – Waveform Generation Mode - to Fast PWM
    // Bit 5,4 – COM01:0 – Compare Match Output Mode - To non-inverted mode
    // Bit 0 - CS20 - enable timer altogether.
    Lib_RegWrite(TCCR2, (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS20) );

    // Clear OCR register.
    Lib_RegClearWhole(OCR2);

    // It is mandatory to set OC2 to output.
    Lib_BitEnable(LIB_PWM_OUTPUT_2_DDR, LIB_PWM_OUTPUT_2_BIT);
}

void Lib_Pwm_Set_2(uint8_t dutyCycle)
{
    Lib_RegWrite(OCR2, dutyCycle);
}

uint8_t Lib_Pwm_Get_2(void)
{
    return Lib_RegRead(OCR2);
}

#endif

void Lib_Pwm_Init(void)
{
#if LIB_PWM_USE_TIMER_1
    InitPwm_1();
#endif
#if LIB_PWM_USE_TIMER_2
    InitPwm_2();
#endif
}

#endif
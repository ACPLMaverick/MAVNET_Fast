#pragma once

#include "defs.h"
#include "pwm_config.h"
#include "timer_config.h"

#if LIB_USE_PWM

// Sanity check for simultaneous timer usage.
#if LIB_TIMER_USE_TIMER_1 && LIB_PWM_USE_TIMER_1
#error "Config error. Cannot use Timer 1 for both scheduling callbacks and PWM."
#endif

#if LIB_TIMER_USE_TIMER_2 && LIB_PWM_USE_TIMER_2
#error "Config error. Cannot use Timer 2 for both scheduling callbacks and PWM."
#endif

void Lib_Pwm_Init(void);

#if LIB_PWM_USE_TIMER_1
#if LIB_PWM_TIMER_1_8BIT_MODE
void Lib_Pwm_Set_1A(uint8_t dutyCycle);
void Lib_Pwm_Set_1B(uint8_t dutyCycle);
uint8_t Lib_Pwm_Get_1A(void);
uint8_t Lib_Pwm_Get_1B(void);
#else
void Lib_Pwm_Set_1A(uint16_t dutyCycle);
void Lib_Pwm_Set_1B(uint16_t dutyCycle);
uint16_t Lib_Pwm_Get_1A(void);
uint16_t Lib_Pwm_Get_1B(void);
#endif
#endif

#if LIB_PWM_USE_TIMER_2
void Lib_Pwm_Set_2(uint8_t dutyCycle);
uint8_t Lib_Pwm_Get_2(void);
#endif

#endif
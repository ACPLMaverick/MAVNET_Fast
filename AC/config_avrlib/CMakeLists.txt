cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

target_sources(avr PUBLIC
    blinker_config.h
    display_config.h
    pwm_config.h
    timer_config.h
    uart_config.h
)
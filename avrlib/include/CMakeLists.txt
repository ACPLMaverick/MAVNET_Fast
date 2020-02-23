cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

if(${LIB_USE_BLINKER})
target_sources(avr PUBLIC
    blinker.h
)
endif()
if(${LIB_USE_DISPLAY})
target_sources(avr PUBLIC
    display.h
)
endif()
if(${LIB_USE_TIMER})
target_sources(avr PUBLIC
    timer.h
)
endif()
if(${LIB_USE_UART})
target_sources(avr PUBLIC
    uart.h
)
endif()
if(${LIB_USE_PWM})
target_sources(avr PUBLIC
    pwm.h
)
endif()

target_sources(avr PUBLIC
    avrlib.h
    cmd_buffer.h
    defs.h
    entry_point.h
)
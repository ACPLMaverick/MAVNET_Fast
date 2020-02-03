cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

if(${LIB_USE_BLINKER})
target_sources(avr PUBLIC
    blinker.c
)
endif()
if(${LIB_USE_DISPLAY})
target_sources(avr PUBLIC
    display.c
)
endif()
if(${LIB_USE_TIMER})
target_sources(avr PUBLIC
    timer.c
)
endif()
if(${LIB_USE_UART})
target_sources(avr PUBLIC
    uart.c
)
endif()

target_sources(avr PUBLIC
    cmd_buffer.c
)
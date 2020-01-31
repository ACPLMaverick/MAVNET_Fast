cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

target_sources(avr PUBLIC
    avrlib.h
    blinker.h
    defs.h
    display.h
    entry_point.h
    timer.h
    uart.h
)
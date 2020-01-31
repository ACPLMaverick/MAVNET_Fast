cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

target_sources(avr PUBLIC
    blinker.c
    display.c
    timer.c
    uart.c
)
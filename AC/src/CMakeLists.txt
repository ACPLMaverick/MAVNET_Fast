cmake_minimum_required (VERSION 3.8)

target_sources(ac PRIVATE
    src/blinker.h
    src/defs.h
    src/timer.h
    src/uart.h

    src/blinker.c
    src/main.c
    src/timer.c
    src/uart.c
)
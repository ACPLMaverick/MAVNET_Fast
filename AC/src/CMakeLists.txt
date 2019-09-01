cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

target_sources(ac PRIVATE
    blinker.h
    defs.h
    timer.h
    uart.h

    blinker.c
    main.c
    timer.c
    uart.c
)
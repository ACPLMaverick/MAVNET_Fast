#pragma once

#include "defs.h"

#define DISP_PIN_RS PORTB4
#define DISP_PIN_RW 0
#define DISP_PIN_E  PORTB5
#define DISP_PIN_D0 0
#define DISP_PIN_D1 0
#define DISP_PIN_D2 0
#define DISP_PIN_D3 0
#define DISP_PIN_D4 PORTB0
#define DISP_PIN_D5 PORTB1
#define DISP_PIN_D6 PORTB2
#define DISP_PIN_D7 PORTB3

#define DISP_REG_DATA       DDRB
#define DISP_REG_CONTROL    DDRB
#define DISP_PORT_DATA      PORTB
#define DISP_PORT_CONTROL   PORTB

#define DISP_MODE_4BIT    1
// TODO other modes.
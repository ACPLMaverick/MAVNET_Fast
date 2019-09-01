#pragma once

#include "defs.h"
#include "display_config.h"

typedef enum Disp_Row
{
    Disp_Row_kUpper,
    Disp_Row_kLower

} Disp_Row;

typedef enum Disp_Alignment
{
    Disp_Alignment_kLeft,
    Disp_Alignment_kCenter,
    Disp_Alignment_kRight

} Disp_Alignment;

void Disp_Init(void);
void Disp_PrintEx(const char* text, uint8_t textLength, Disp_Row row, Disp_Alignment alignment);
#define Disp_Print(text, row, alignment) Disp_PrintEx((text), (uint8_t)strlen(text), (row), (alignment))
void Disp_Printf(Disp_Row row, Disp_Alignment alignment, const char* format, ...);

// TODO Print custom form
// TODO Scroll display L-R
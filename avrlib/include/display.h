#pragma once

#include "defs.h"

typedef enum Lib_Disp_Row
{
    Lib_Disp_Row_kUpper,
    Lib_Disp_Row_kLower

} Lib_Disp_Row;

typedef enum Lib_Disp_Alignment
{
    Lib_Disp_Alignment_kLeft,
    Lib_Disp_Alignment_kCenter,
    Lib_Disp_Alignment_kRight

} Lib_Disp_Alignment;

void Lib_Disp_Init(void);
void Lib_Disp_Tick(void);

void Lib_Disp_Clear(void);
void Lib_Disp_ClearRow(Lib_Disp_Row row);

void Lib_Disp_On(void);
void Lib_Disp_Off(void);

void Lib_Disp_PrintCopyEx(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* text, uint8_t charNum);
#define Lib_Disp_PrintCopy(row, alignment, text) Lib_Disp_PrintCopyEx(row, alignment, text, strlen(text))
void Lib_Disp_PrintEx(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* text, uint8_t charNum);
#define Lib_Disp_Print(row, alignment, text) Lib_Disp_PrintEx(row, alignment, text, strlen(text))
void Lib_Disp_Printf(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* format, ...);

// TODO Print custom form
// TODO Scroll display L-R
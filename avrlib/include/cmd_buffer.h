#pragma once

#include "defs.h"

typedef enum Lib_CmdType
{
    Lib_CmdType_kCommand = 0,
    Lib_CmdType_kDataBuffer
} Lib_CmdType;

typedef enum Lib_CmdState
{
    Lib_CmdState_kInvalid = 0,
    Lib_CmdState_kPending

} Lib_CmdState;

typedef struct Lib_Cmd
{
    uint16_t m_value    : 16;
    Lib_CmdState m_state    : 4;
    Lib_CmdType m_type      : 4;

} Lib_Cmd;

// Callback gets the next command to process as a parameter.
// Return value: True - command buffer should proceed - the command
// has been fully processed. False - vice versa.
typedef bool(*Lib_CmdPerformFunc)(Lib_Cmd*);

typedef struct Lib_CmdBuffer
{
#define LIB_CMD_BUFFER_SIZE 32
    Lib_Cmd m_commands[LIB_CMD_BUFFER_SIZE];
    Lib_Cmd* m_ptrPerform;
    Lib_Cmd* m_ptrInsert;
    Lib_CmdPerformFunc m_func;

} Lib_CmdBuffer;

void Lib_CmdBuffer_Init(Lib_CmdBuffer* buffer, Lib_CmdPerformFunc performFunc);
void Lib_CmdBuffer_Insert(Lib_CmdBuffer* buffer, uint16_t value, Lib_CmdType type);
void Lib_CmdBuffer_Proceed(Lib_CmdBuffer* buffer, Lib_Cmd** ptr);
void Lib_CmdBuffer_Flush(Lib_CmdBuffer* buffer);
#define Lib_CmdBuffer_ProceedPerform(buffer) Lib_CmdBuffer_Proceed(buffer, &buffer->m_ptrPerform)
#define Lib_CmdBuffer_ProceedInsert(buffer) Lib_CmdBuffer_Proceed(buffer, &buffer->m_ptrInsert)
void Lib_CmdBuffer_Perform(Lib_CmdBuffer* buffer);
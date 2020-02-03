#include "cmd_buffer.h"

#define CMD_BUFFER_DEBUG 0

#if CMD_BUFFER_DEBUG
#include "uart.h"
#define Cmd_DebugPrintf Lib_Uart_Printf
#else
#define Cmd_DebugPrintf(...)
#endif

void Lib_CmdBuffer_Init(Lib_CmdBuffer* buffer, Lib_CmdPerformFunc performFunc)
{
    memset(buffer->m_commands, 0, LIB_CMD_BUFFER_SIZE * sizeof(buffer->m_commands[0]));
    buffer->m_ptrPerform = buffer->m_commands;
    buffer->m_ptrInsert = buffer->m_commands;
    buffer->m_func = performFunc;
}

void Lib_CmdBuffer_Insert(Lib_CmdBuffer* buffer, uint16_t value, Lib_CmdType type)
{
    if(buffer->m_ptrPerform->m_state != Lib_CmdState_kInvalid
        && buffer->m_ptrInsert == buffer->m_ptrPerform)
    {
        Cmd_DebugPrintf("[DISP] [WARN] Command overflow with command: 0x%x %d. Skipping.\n", value, type);
        return;
    }

    buffer->m_ptrInsert->m_value = value;
    buffer->m_ptrInsert->m_type = type;
    buffer->m_ptrInsert->m_state = Lib_CmdState_kPending;

    Lib_CmdBuffer_ProceedInsert(buffer);
}

void Lib_CmdBuffer_Proceed(Lib_CmdBuffer* buffer, Lib_Cmd** ptr)
{
    ++(*ptr);
    if(*ptr >= (buffer->m_commands + LIB_CMD_BUFFER_SIZE))
    {
        *ptr = buffer->m_commands;
    }
}


void Lib_CmdBuffer_Flush(Lib_CmdBuffer* buffer)
{
    while(buffer->m_ptrPerform->m_state != Lib_CmdState_kInvalid)
    {
        Lib_CmdBuffer_Perform(buffer);
    }
}

void Lib_CmdBuffer_Perform(Lib_CmdBuffer* buffer)
{
    if(buffer->m_func(buffer->m_ptrPerform))
    {
        // Update pointer to point to the next command.
        Lib_CmdBuffer_ProceedPerform(buffer);
    }
}
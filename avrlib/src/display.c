#include "display.h"
#include "display_config.h"
#include "timer.h"

#include <stdio.h>

#define DISP_DEBUG 0

#if DISP_DEBUG
#include "uart.h"
#define Disp_DebugPrintf Lib_Uart_Printf
#else
#define Disp_DebugPrintf(...)
#endif

#define DISP_PORT_BF    (LIB_DISP_PORT_D0 + 3)
#define DISP_PIN_BF     (LIB_DISP_PIN_D0 + 3)

// Display config params

#define DISP_COL_COUNT 16
#define Lib_Disp_Row_COUNT 2

#define Lib_Disp_ClearDISPLAY   0x01
#define DISP_RETURNHOME     0x02
#define DISP_ENTRYMODESET   0x04
#define DISP_DISPLAYCONTROL 0x08
#define DISP_CURSORSHIFT    0x10
#define DISP_FUNCTIONSET    0x20
#define DISP_SETCGRAMADDR   0x40
#define DISP_SETDDRAMADDR   0x80

#define DISP_ENTRYRIGHT          0x00
#define DISP_ENTRYLEFT           0x02
#define DISP_ENTRYSHIFTINCREMENT 0x01
#define DISP_ENTRYSHIFTDECREMENT 0x00

#define DISP_DISPLAYON  0x04
#define DISP_DISPLAYOFF 0x00
#define DISP_CURSORON   0x02
#define DISP_CURSOROFF  0x00
#define DISP_BLINKON    0x01
#define DISP_BLINKOFF   0x00

#define DISP_DISPLAYMOVE 0x08
#define DISP_CURSORMOVE  0x00
#define DISP_MOVERIGHT   0x04
#define DISP_MOVELEFT    0x00

#define DISP_8BITMODE 0x10
#define DISP_4BITMODE 0x00
#define DISP_2LINE    0x08
#define DISP_1LINE    0x00
#define DISP_5x10DOTS 0x04
#define DISP_5x8DOTS  0x00

// End display config params


// Structs

typedef enum CmdType
{
    CmdType_kCommand = 0,
    CmdType_kDataBuffer
} CmdType;

typedef enum CmdState
{
    CmdState_kInvalid = 0,
    CmdState_kPending

} CmdState;

typedef struct Cmd
{
    uint16_t m_value    : 16;
    CmdState m_state    : 4;
    CmdType m_type      : 4;

} Cmd;

typedef struct CmdBuffer
{
#define CMD_BUFFER_SIZE 32
    Cmd m_commands[CMD_BUFFER_SIZE];
    Cmd* m_ptrPerform;
    Cmd* m_ptrInsert;

} CmdBuffer;

typedef struct DisplayState
{
    uint8_t m_displayFlag   : 8;
    uint8_t m_cursorPos     : 8;
    bool m_dataRegister     : 4;
    bool m_read             : 4;

} DisplayState;

// End structs


// Globals

char g_displayBuffer[DISP_COL_COUNT + 1][Lib_Disp_Row_COUNT];
char g_printfBuffer[DISP_COL_COUNT + 1];
CmdBuffer g_cmdBuffer;
DisplayState g_displayState;

// End globals


static inline void SetInstructionRegister(void);
static inline void SetDataRegister(void);
static inline void SetWrite(void);
static inline void SetRead(void);
static inline void SendCommand(uint8_t command);
static inline void SendText(const char* text);
static inline void WriteHalfByte(uint8_t nibble);
static inline void WriteByte(uint8_t byte);
static inline void BeginTransfer(void);
static inline void EndTransfer(void);
static inline bool TryBusyFlag(void);
static inline void CmdBuffer_Init(CmdBuffer* buffer);
static inline void CmdBuffer_Insert(CmdBuffer* buffer, uint16_t value, CmdType type);
static inline void CmdBuffer_Proceed(CmdBuffer* buffer, Cmd** ptr);
static inline void CmdBuffer_Flush(CmdBuffer* buffer);
#define CmdBuffer_ProceedPerform(buffer) CmdBuffer_Proceed(buffer, &buffer->m_ptrPerform)
#define CmdBuffer_ProceedInsert(buffer) CmdBuffer_Proceed(buffer, &buffer->m_ptrInsert)
static inline void CmdBuffer_Perform(CmdBuffer* buffer);

static inline void ConfigurePins(void);
static inline void MysteriousInit(void);
static void CalculateTextPosition(Lib_Disp_Row row, Lib_Disp_Alignment alignment, uint8_t* textLength, uint8_t* outPosX, uint8_t* outPosY);
static inline uint8_t CalculateCursorPosition(const uint8_t posX, const uint8_t posY);
static inline void SetCursor(const uint8_t posX, const uint8_t posY);
static void Print_Internal(const uint8_t textPosX, const uint8_t textPosY, const char* text, uint8_t charNum);


static inline void SetInstructionRegister(void)
{
    if(g_displayState.m_dataRegister)
    {
        Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RS);
        g_displayState.m_dataRegister = false;
        Disp_DebugPrintf("[DISP] Setting Instruction Register.\n");
    }
}

static inline void SetDataRegister(void)
{
    if(!g_displayState.m_dataRegister)
    {
        Lib_BitEnable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RS);
        g_displayState.m_dataRegister = true;
        Disp_DebugPrintf("[DISP] Setting Data Register.\n");
    }
}

static inline void SetWrite(void)
{
    if(g_displayState.m_read)
    {
        Lib_BitEnable(LIB_DISP_DDR_DATA, DISP_PORT_BF);  // Update busy flag pinout as write.
        Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RW);
        g_displayState.m_read = false;
        Disp_DebugPrintf("[DISP] Setting Write.\n");
    }
}

static inline void SetRead(void)
{
    if(!g_displayState.m_read)
    {
        Lib_BitDisable(LIB_DISP_DDR_DATA, DISP_PORT_BF);  // Update busy flag pinout as read.
        Lib_BitEnable(LIB_DISP_PORT_DATA, DISP_PORT_BF);  // Enable pull-up read mode.
        Lib_BitEnable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RW);
        g_displayState.m_read = true;
        Disp_DebugPrintf("[DISP] Setting Read.\n");
    }
}

static inline void SendCommand(uint8_t command)
{
    CmdBuffer_Insert(&g_cmdBuffer, (uint16_t)command, CmdType_kCommand);
}

static inline void SendText(const char* text)
{
    CmdBuffer_Insert(&g_cmdBuffer, (uint16_t)text, CmdType_kDataBuffer);
}

static inline void WriteHalfByte(uint8_t nibble)
{
    BeginTransfer();
    Lib_RegWriteHalfByte(LIB_DISP_PORT_DATA, LIB_DISP_PORT_D0, nibble);
    EndTransfer();
}

static inline void WriteByte(uint8_t byte)
{
    WriteHalfByte(byte >> 4);
    WriteHalfByte(byte);
}

static inline void BeginTransfer(void)
{
    Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_E);
    Lib_BitEnable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_E);
}

static inline void EndTransfer(void)
{
    Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_E);
}

static inline bool TryBusyFlag(void)
{
    SetRead();
    SetInstructionRegister();

    BeginTransfer();

    const bool val = Lib_BitRead(LIB_DISP_PIN_DATA, DISP_PIN_BF);
    Disp_DebugPrintf("[DISP] TryBusyFlag [%d]\n", val);

    EndTransfer();

    BeginTransfer();   // Second nibble - we don't need it, but for safety we fake reading whole byte.
    EndTransfer();

    return !val;
}

static inline void CmdBuffer_Init(CmdBuffer* buffer)
{
    memset(buffer->m_commands, 0, CMD_BUFFER_SIZE * sizeof(buffer->m_commands[0]));
    buffer->m_ptrPerform = buffer->m_commands;
    buffer->m_ptrInsert = buffer->m_commands;
}

static inline void CmdBuffer_Insert(CmdBuffer* buffer, uint16_t value, CmdType type)
{
    if(buffer->m_ptrPerform->m_state != CmdState_kInvalid
        && buffer->m_ptrInsert == buffer->m_ptrPerform)
    {
        Disp_DebugPrintf("[DISP] [WARN] Command overflow with command: 0x%x %d. Skipping.\n", value, type);
        return;
    }

    buffer->m_ptrInsert->m_value = value;
    buffer->m_ptrInsert->m_type = type;
    buffer->m_ptrInsert->m_state = CmdState_kPending;

    CmdBuffer_ProceedInsert(buffer);
}

static inline void CmdBuffer_Proceed(CmdBuffer* buffer, Cmd** ptr)
{
    ++(*ptr);
    if(*ptr >= (buffer->m_commands + CMD_BUFFER_SIZE))
    {
        *ptr = buffer->m_commands;
    }
}

static inline void CmdBuffer_Perform(CmdBuffer* buffer)
{
    // Get next command
    Cmd* cmd = buffer->m_ptrPerform;
    if(cmd->m_state == CmdState_kInvalid)   // Pointer points to invalid command - no commands to process.
    {
        return;
    }

    if(!TryBusyFlag())
    {
        return;
    }

    SetWrite(); // After waiting for busy flag.

    switch (cmd->m_type)
    {
    case CmdType_kCommand:
    {
        SetInstructionRegister();

        uint8_t value = (uint8_t)cmd->m_value;
        if(cmd->m_state == CmdState_kPending)
        {
            WriteByte(value);
            Disp_DebugPrintf("[DISP] Sending Display Command: [0x%x]\n", value);

            // Clear state.
            cmd->m_state = CmdState_kInvalid;

            // Update pointer to point to the next command.
            CmdBuffer_ProceedPerform(buffer);
        }
    }
        break;
    
    case CmdType_kDataBuffer:
    {
        SetDataRegister();

        char letter = *((char*)cmd->m_value);
        if(letter == 0)
        {
            // We have finished writing this string. Clear state and update pointer to the next cmd.
            cmd->m_state = CmdState_kInvalid;
            CmdBuffer_ProceedPerform(buffer);
        }
        else if(cmd->m_state == CmdState_kPending)
        {
            WriteByte(letter);
            Disp_DebugPrintf("[DISP] Sending Display Char: %c\n", letter);

            // Update state to first nibble.
            cmd->m_state = CmdState_kPending;

            // Increment pointer to string.
            ++cmd->m_value;
        }
    }
        break;
    
    default:
        break;
    }
}

static inline void CmdBuffer_Flush(CmdBuffer* buffer)
{
    while(buffer->m_ptrPerform->m_state != CmdState_kInvalid)
    {
        CmdBuffer_Perform(buffer);
    }
}

static inline void ConfigurePins(void)
{
    // Control goes all as output.
    Lib_BitEnable(LIB_DISP_DDR_CONTROL, LIB_DISP_PORT_RS);
    Lib_BitEnable(LIB_DISP_DDR_CONTROL, LIB_DISP_PORT_RW);
    Lib_BitEnable(LIB_DISP_DDR_CONTROL, LIB_DISP_PORT_E);

    // Same with data.
    Lib_BitEnable(LIB_DISP_DDR_DATA, LIB_DISP_PORT_D0);
    Lib_BitEnable(LIB_DISP_DDR_DATA, LIB_DISP_PORT_D0 + 1);
    Lib_BitEnable(LIB_DISP_DDR_DATA, LIB_DISP_PORT_D0 + 2);
    Lib_BitEnable(LIB_DISP_DDR_DATA, LIB_DISP_PORT_D0 + 3);

    // Wait 15 ms for DISP to initialize, as stated in doc.
    Lib_Timer_SleepMs(15);

    Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RS);
    Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_RW);
    Lib_BitDisable(LIB_DISP_PORT_CONTROL, LIB_DISP_PORT_E);
}

static inline void MysteriousInit(void)
{
    Lib_Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // Switch to 4 bit mode
    Lib_Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // 2nd time
    Lib_Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // 3rd time
    Lib_Timer_SleepMs(4.1);

    // Ext lib says "Set 8-bit mode (?)" But I don't fuckin' know what it is, all I know is that it totally doesn't work without that...
    WriteHalfByte(0x02);
    Lib_Timer_SleepMs(4.1);
}

static void CalculateTextPosition(Lib_Disp_Row row, Lib_Disp_Alignment alignment, uint8_t* refTextLength, uint8_t* outPosX, uint8_t* outPosY)
{
    *outPosY = (uint8_t)row;
    *outPosX = 0;

    if(*refTextLength >= DISP_COL_COUNT)
    {
        *refTextLength = DISP_COL_COUNT;
        // Let's always adjust to the left if text exceeds display.
    }
    else
    {
        switch (alignment)
        {
        case Lib_Disp_Alignment_kCenter:
            {
                const uint8_t offset = (DISP_COL_COUNT - *refTextLength) / 2;
                *outPosX += offset;
            }
            break;

        case Lib_Disp_Alignment_kRight:
            {
                const uint8_t offset = (DISP_COL_COUNT - *refTextLength);
                *outPosX += offset;
            }
            break;
        
        default:
            break;
        }
    }
}

static inline uint8_t CalculateCursorPosition(const uint8_t posX, const uint8_t posY)
{
    static const uint8_t rowOffsets[] = {0x00, 0x40};
    return (rowOffsets[(uint8_t)posY]) + posX;
}

static inline void SetCursor(const uint8_t posX, const uint8_t posY)
{
    const uint8_t cursorPos = CalculateCursorPosition(posX, posY);
    g_displayState.m_cursorPos = cursorPos;
    SendCommand(DISP_SETDDRAMADDR | cursorPos);
}

static void Print_Internal(const uint8_t textPosX, const uint8_t textPosY, const char* text, uint8_t charNum)
{
    SetCursor(textPosX, textPosY);
    SendText(text);
}


void Lib_Disp_Init(void)
{
    memset(g_printfBuffer, 0, sizeof(g_printfBuffer));
    memset(&g_displayState, 0, sizeof(DisplayState));

    ConfigurePins();

    MysteriousInit();

    CmdBuffer_Init(&g_cmdBuffer);

    // Init default settings.

    SendCommand(DISP_FUNCTIONSET | DISP_4BITMODE | DISP_2LINE | DISP_5x8DOTS);
    g_displayState.m_displayFlag = DISP_CURSOROFF | DISP_BLINKOFF;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);

    Lib_Disp_Clear();
}

void Lib_Disp_Tick(void)
{
    CmdBuffer_Perform(&g_cmdBuffer);
}

void Lib_Disp_Clear(void)
{
    CmdBuffer_Flush(&g_cmdBuffer);

    memset(g_displayBuffer, 0, DISP_COL_COUNT * Lib_Disp_Row_COUNT * sizeof(g_displayBuffer[0]));
    CmdBuffer_Init(&g_cmdBuffer);

    g_displayState.m_cursorPos = 0;
    SendCommand(Lib_Disp_ClearDISPLAY);
}

void Lib_Disp_ClearRow(Lib_Disp_Row row)
{
    CmdBuffer_Flush(&g_cmdBuffer);

    SetCursor(0, (uint8_t)row);

    char* rowText = &g_displayBuffer[0][(uint8_t)row];
    memset(rowText, ' ', DISP_COL_COUNT);
    rowText[DISP_COL_COUNT] = 0;
    SendText(rowText);
}

void Lib_Disp_On(void)
{
    g_displayState.m_displayFlag |= DISP_DISPLAYON;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);
}

void Lib_Disp_Off(void)
{
    g_displayState.m_displayFlag &= ~DISP_DISPLAYON;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);
}

void Lib_Disp_PrintCopyEx(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* text, uint8_t charNum)
{
    uint8_t textPosX, textPosY;
    CalculateTextPosition(row, alignment, &charNum, &textPosX, &textPosY);

    char* textDst = &g_displayBuffer[textPosY][textPosX];
    strncpy(textDst, text, charNum);

    Print_Internal(textPosX, textPosY, textDst, charNum);
}

void Lib_Disp_PrintEx(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* text, uint8_t charNum)
{
    uint8_t textPosX, textPosY;
    CalculateTextPosition(row, alignment, &charNum, &textPosX, &textPosY);

    Print_Internal(textPosX, textPosY, text, charNum);
}

void Lib_Disp_Printf(Lib_Disp_Row row, Lib_Disp_Alignment alignment, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char* textDst = g_printfBuffer;
    int charNum = vsnprintf(textDst, DISP_COL_COUNT, format, args);

    if(charNum <= 0)
    {
        if(charNum < 0)
        {
            Disp_DebugPrintf("[DISP] Error printing: %s \n", format);
        }
        return;
    }

    va_end(args);

    Lib_Disp_PrintCopyEx(row, alignment, textDst, charNum);
}
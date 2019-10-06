#include "display.h"
#include "timer.h"

#include <stdio.h>

#define DISP_DEBUG 1

#if DISP_DEBUG
#include "uart.h"
#define Disp_DebugPrintf Uart_Printf
#else
#define Disp_DebugPrintf(...)
#endif


// Display config params

#define DISP_COL_COUNT 16
#define DISP_ROW_COUNT 2

#define DISP_CLEARDISPLAY   0x01
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

#if DISP_MODE_4BIT
#define DISP_MODE_8BIT 0
#define DISP_CURRENT_MODE_FLAG DISP_4BITMODE
#define DISP_PIN_BF (DISP_PIN_D0 + 3)
#else
#define DISP_MODE_8BIT 1
#define DISP_CURRENT_MODE_FLAG DISP_8BITMODE
#define DISP_PIN_BF (DISP_PIN_D0 + 7)
#endif

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

char g_displayBuffer[DISP_COL_COUNT + 1][DISP_ROW_COUNT];
char g_printfBuffer[DISP_COL_COUNT + 1];
char g_spaceBuffer[DISP_COL_COUNT + 1];
CmdBuffer g_cmdBuffer;
DisplayState g_displayState;

// End globals


static inline void SetInstructionRegister(void);
static inline void SetDataRegister(void);
static inline void SetWrite(void);
static inline void SetRead(void);
static inline void SendCommand(uint8_t command);
static inline void SendText(const char* text);
#if DISP_MODE_4BIT
static inline void WriteHalfByte(uint8_t nibble);
#endif
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
static void CalculateTextPosition(Disp_Row row, Disp_Alignment alignment, uint8_t textLength, uint8_t* outPosX, uint8_t* outPosY);
static inline uint8_t CalculateCursorPosition(const uint8_t posX, const uint8_t posY);
static inline void SetCursor(const uint8_t posX, const uint8_t posY);
static void Print_Internal(const uint8_t textPosX, const uint8_t textPosY, const char* text, uint8_t charNum);


static inline void SetInstructionRegister(void)
{
    if(g_displayState.m_dataRegister)
    {
        BitDisable(DISP_PORT_CONTROL, DISP_PIN_RS);
        g_displayState.m_dataRegister = false;
        Disp_DebugPrintf("[DISP] Setting Instruction Register.\n");
    }
}

static inline void SetDataRegister(void)
{
    if(!g_displayState.m_dataRegister)
    {
        BitEnable(DISP_PORT_CONTROL, DISP_PIN_RS);
        g_displayState.m_dataRegister = true;
        Disp_DebugPrintf("[DISP] Setting Data Register.\n");
    }
}

static inline void SetWrite(void)
{
    if(g_displayState.m_read)
    {
        BitEnable(DISP_REG_DATA, DISP_PIN_BF);  // Update busy flag pinout as write.
        BitDisable(DISP_PORT_CONTROL, DISP_PIN_RW);
        g_displayState.m_read = false;
        Disp_DebugPrintf("[DISP] Setting Write.\n");
    }
}

static inline void SetRead(void)
{
    if(!g_displayState.m_read)
    {
        BitDisable(DISP_REG_DATA, DISP_PIN_BF);  // Update busy flag pinout as read.
        BitEnable(DISP_PORT_CONTROL, DISP_PIN_RW);
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

#if DISP_MODE_4BIT
static inline void WriteHalfByte(uint8_t nibble)
{
    BeginTransfer();
    RegWriteHalfByte(DISP_PORT_DATA, DISP_PIN_D0, nibble);
    EndTransfer();
}
#endif

static inline void WriteByte(uint8_t byte)
{
#if DISP_MODE_4BIT
    WriteHalfByte(byte >> 4);
    WriteHalfByte(byte);
#else
    RegWrite(DISP_PORT_DATA, DISP_PIN_D0, byte);
#endif
}

static inline void BeginTransfer(void)
{
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_E);
    BitEnable(DISP_PORT_CONTROL, DISP_PIN_E);
}

static inline void EndTransfer(void)
{
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_E);
}

#define TRY_BUSY_FLAG_FAKE 1

static inline bool TryBusyFlag(void)
{
#if TRY_BUSY_FLAG_FAKE
    
    Timer_SleepMs(1);
    return true;

#else

    SetRead();
    SetInstructionRegister();

    BeginTransfer();

    const bool val = BitRead(DISP_PORT_DATA, DISP_PIN_BF);
    Disp_DebugPrintf("[DISP] TryBusyFlag [%d]\n", val);

    EndTransfer();

    BeginTransfer();   // Second nibble - we don't need it, but for safety we fake reading whole byte.
    EndTransfer();

    return !val;

#endif
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
    BitEnable(DISP_REG_CONTROL, DISP_PIN_RS);
    BitEnable(DISP_REG_CONTROL, DISP_PIN_RW);
    BitEnable(DISP_REG_CONTROL, DISP_PIN_E);

    // Same with data.
    BitEnable(DISP_REG_DATA, DISP_PIN_D0);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 1);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 2);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 3);
#if DISP_MODE_8BIT
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 4);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 5);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 6);
    BitEnable(DISP_REG_DATA, DISP_PIN_D0 + 7);
#endif

    // Wait 15 ms for DISP to initialize, as stated in doc.
    Timer_SleepMs(15);

    BitDisable(DISP_PORT_CONTROL, DISP_PIN_RS);
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_RW);
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_E);
}

static inline void MysteriousInit(void)
{
    Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // Switch to 4 bit mode
    Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // 2nd time
    Timer_SleepMs(4.1);

    WriteHalfByte(0x03); // 3rd time
    Timer_SleepMs(4.1);

    // Ext lib says "Set 8-bit mode (?)" But I don't fuckin' know what it is, all I know is that it totally doesn't work without that...
    WriteHalfByte(0x02);
    Timer_SleepMs(4.1);
}

static void CalculateTextPosition(Disp_Row row, Disp_Alignment alignment, uint8_t textLength, uint8_t* outPosX, uint8_t* outPosY)
{
    *outPosY = (uint8_t)row;
    *outPosX = 0;

    switch (alignment)
    {
    case Disp_Alignment_kCenter:
            *outPosX += (DISP_COL_COUNT - textLength) / 2;
        break;

    case Disp_Alignment_kRight:
            *outPosX += (DISP_COL_COUNT - textLength);
        break;
    
    default:
        break;
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


void Disp_Init(void)
{
    memset(g_printfBuffer, 0, sizeof(g_printfBuffer));
    memset(g_spaceBuffer, ' ', sizeof(g_spaceBuffer));
    g_spaceBuffer[DISP_COL_COUNT] = 0;
    memset(&g_displayState, 0, sizeof(DisplayState));

    ConfigurePins();

    MysteriousInit();

    CmdBuffer_Init(&g_cmdBuffer);

    // Init default settings.

    SendCommand(DISP_FUNCTIONSET | DISP_CURRENT_MODE_FLAG | DISP_2LINE | DISP_5x8DOTS);
    g_displayState.m_displayFlag = DISP_CURSOROFF | DISP_BLINKOFF;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);

    Disp_Clear();
}

void Disp_Tick(void)
{
    CmdBuffer_Perform(&g_cmdBuffer);
}

void Disp_Clear(void)
{
    CmdBuffer_Flush(&g_cmdBuffer);

    memset(g_displayBuffer, 0, DISP_COL_COUNT * DISP_ROW_COUNT * sizeof(g_displayBuffer[0]));
    CmdBuffer_Init(&g_cmdBuffer);

    g_displayState.m_cursorPos = 0;
    SendCommand(DISP_CLEARDISPLAY);
}

void Disp_ClearRow(Disp_Row row)
{
    SetCursor(0, (uint8_t)row);
    SendText(g_spaceBuffer);
}

void Disp_On(void)
{
    g_displayState.m_displayFlag |= DISP_DISPLAYON;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);
}

void Disp_Off(void)
{
    g_displayState.m_displayFlag &= ~DISP_DISPLAYON;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState.m_displayFlag);
}

void Disp_PrintCopyEx(Disp_Row row, Disp_Alignment alignment, const char* text, uint8_t charNum)
{
    if(charNum > DISP_COL_COUNT)
    {
        charNum = DISP_COL_COUNT;
    }

    uint8_t textPosX, textPosY;
    CalculateTextPosition(row, alignment, charNum, &textPosX, &textPosY);

    char* textDst = &g_displayBuffer[textPosY][textPosX];
    strncpy(textDst, text, charNum);

    Print_Internal(textPosX, textPosY, textDst, charNum);
}

void Disp_PrintEx(Disp_Row row, Disp_Alignment alignment, const char* text, uint8_t charNum)
{
    if(charNum > DISP_COL_COUNT)
    {
        charNum = DISP_COL_COUNT;
    }

    uint8_t textPosX, textPosY;
    CalculateTextPosition(row, alignment, charNum, &textPosX, &textPosY);

    Print_Internal(textPosX, textPosY, text, charNum);
}

void Disp_Printf(Disp_Row row, Disp_Alignment alignment, const char* format, ...)
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

    Disp_PrintCopyEx(row, alignment, textDst, charNum);
}
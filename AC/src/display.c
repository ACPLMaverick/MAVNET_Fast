#include "display.h"
#include "timer.h"

#include <stdio.h>

#define DISP_DEBUG 0

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
#else
#define DISP_MODE_8BIT 1
#define DISP_CURRENT_MODE_FLAG DISP_8BITMODE
#endif

// End display config params


// Globals

char g_displayBuffer[DISP_COL_COUNT + 1][DISP_ROW_COUNT];
uint8_t g_displayState;
uint8_t g_cursorPos;

// End globals


static inline void SetInstructionRegister(void);
static inline void SetDataRegister(void);
static inline void SetWrite(void);
static inline void SetRead(void);
static inline void SendCommand(uint8_t command);
static inline void SendData(uint8_t data);
#if DISP_MODE_4BIT
static inline void WriteHalfByte(uint8_t nibble);
#elif DISP_MODE_8BIT
static inline void WriteByte(uint8_t byte);
#endif

static inline void ConfigurePins(void);
static inline void MysteriousInit(void);
static inline void SetCursor(Disp_Row row, Disp_Alignment alignment, uint8_t textLength);


static inline void SetInstructionRegister(void)
{
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_RS);
}

static inline void SetDataRegister(void)
{
    BitEnable(DISP_PORT_CONTROL, DISP_PIN_RS);
}

static inline void SetWrite(void)
{
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_RW);
}

static inline void SetRead(void)
{
    BitEnable(DISP_PORT_CONTROL, DISP_PIN_RW);
}

static inline void SendCommand(uint8_t command)
{
    SetInstructionRegister();
    SetWrite();

    Disp_DebugPrintf("Sending Display Command: %d \n", command);

    WriteHalfByte(command >> 4);
    WriteHalfByte(command);
}

static inline void SendData(uint8_t data)
{
    SetDataRegister();
    SetWrite();

    Disp_DebugPrintf("Sending Display Data: %c \n", data);

    WriteHalfByte(data >> 4);
    WriteHalfByte(data);
}

#if DISP_MODE_4BIT
static inline void WriteHalfByte(uint8_t nibble)
{
    RegWriteHalfByte(DISP_PORT_DATA, DISP_PIN_D0, nibble);

    // Clear any previous state, set 1, and then clear.
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_E);
    BitEnable(DISP_PORT_CONTROL, DISP_PIN_E);
    BitDisable(DISP_PORT_CONTROL, DISP_PIN_E);

    Timer_SleepMs(0.3);
}
#elif DISP_MODE_8BIT
static inline void WriteByte(uint8_t byte)
{
#error "TODO"
}
#endif

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
    _delay_ms(4.1);

    WriteHalfByte(0x03); // Switch to 4 bit mode
    _delay_ms(4.1);

    WriteHalfByte(0x03); // 2nd time
    _delay_ms(4.1);

    WriteHalfByte(0x03); // 3rd time
    _delay_ms(4.1);

    // Ext lib says "Set 8-bit mode (?)" But I don't fuckin' know what it is, all I know is that it totally doesn't work without that...
    WriteHalfByte(0x02);
}

static inline void SetCursor(Disp_Row row, Disp_Alignment alignment, uint8_t textLength)
{
    static uint8_t rowOffsets[] = {0x00, 0x40};

    uint8_t cursorPos = (rowOffsets[(uint8_t)row]);

    switch (alignment)
    {
    case Disp_Alignment_kCenter:
        {
            const uint8_t halfDiff = (DISP_COL_COUNT - textLength) / 2;
            cursorPos += halfDiff;
        }
        break;

    case Disp_Alignment_kRight:
        {
            const uint8_t diff = (DISP_COL_COUNT - textLength);
            cursorPos += diff;
        }
        break;
    
    default:
        break;
    }

    g_cursorPos = cursorPos;
    SendCommand(DISP_SETDDRAMADDR | cursorPos);
}

void Disp_Init(void)
{
    ConfigurePins();

    MysteriousInit();

    // Init default settings.

    SendCommand(DISP_FUNCTIONSET | DISP_CURRENT_MODE_FLAG | DISP_2LINE | DISP_5x8DOTS);
    g_displayState = DISP_CURSOROFF | DISP_BLINKOFF | DISP_DISPLAYON;
    SendCommand(DISP_DISPLAYCONTROL | g_displayState);

    Disp_Clear();
}

void Disp_Tick(void)
{

}

void Disp_Clear(void)
{
    g_cursorPos = 0;
    SendCommand(DISP_CLEARDISPLAY);
    Timer_SleepMs(2);
}

void Disp_ClearRow(Disp_Row row)
{
    SetCursor(row, Disp_Alignment_kLeft, DISP_COL_COUNT);
    for(uint8_t i = 0; i < DISP_COL_COUNT; ++i)
    {
        SendData((uint8_t)' ');
    }
}

void Disp_PrintEx(Disp_Row row, Disp_Alignment alignment, const char* text, uint8_t charNum)
{
    if(charNum > DISP_COL_COUNT)
    {
        charNum = DISP_COL_COUNT;
    }

    SetCursor(row, alignment, charNum);

    for(uint8_t i = 0; i < charNum; ++i)
    {
        SendData((uint8_t)text[i]);
    }
}

void Disp_Printf(Disp_Row row, Disp_Alignment alignment, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char* textDst = g_displayBuffer[(uint8_t)row];
    int charNum = vsnprintf(textDst, DISP_COL_COUNT, format, args);
    if(charNum > DISP_COL_COUNT)
    {
        charNum = DISP_COL_COUNT;
    }

    va_end(args);

    Disp_PrintEx(row, alignment, textDst, charNum);
}
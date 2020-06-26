#include "platform.h"

#if JE_PLATFORM_WINDOWS

#include <strsafe.h>

#include "global.h"

namespace je { namespace platform {

void print_last_error()
{
    LPVOID msg_buf;
    DWORD error = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &msg_buf,
        0, NULL );

    JE_printf("Windows error [%ld] : %s", error, reinterpret_cast<char*>(msg_buf));

    LocalFree(msg_buf);
}

}}

#endif
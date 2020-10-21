#include "platf_windows.h"

#if JE_PLATFORM_WINDOWS

#include "global.h"

namespace je { namespace platf {

namespace util
{
    // Cross-platform implementations.
    data::string get_file_separator()
    {
        return data::string("\\");
    }

    data::string call_system_command(const data::string& command)
    {
        JE_todo();
        return data::string();
    }

    void debugbreak()
    {
        __debugbreak();
    }

    // Platform-specific implementations.
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

        JE_print("Windows error [%ld] : %s", error, reinterpret_cast<char*>(msg_buf));

        LocalFree(msg_buf);
    }
}

}}

#endif
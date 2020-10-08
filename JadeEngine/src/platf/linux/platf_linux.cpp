#include "platf/platf.h"
#include "platf_linux.h"

#if JE_PLATFORM_LINUX

#include "global.h"

namespace je { namespace platf {

namespace util
{
    // Cross-platform implementations.
    const char* get_file_separator()
    {
        return "/";
    }

    void debugbreak()
    {
        raise(SIGTRAP);
    }

    // Platform-specific implementations.
    void print_last_error()
    {
        JE_todo();
    }
}

}}

#endif
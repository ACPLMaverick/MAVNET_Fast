#include "util/misc.h"

#if JE_PLATFORM_WINDOWS

#include "global.h"

namespace je { namespace util {

    // Cross-platform implementations.
    data::string misc::call_system_command(const data::string& command)
    {
        JE_todo();
        return data::string();
    }

    void misc::debugbreak()
    {
        __debugbreak();
    }

}}

#endif
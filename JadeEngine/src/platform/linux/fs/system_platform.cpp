#include "fs/system.h"

#if JE_PLATFORM_LINUX

namespace je { namespace fs {

    void system::mount()
    {
        return;
    }

    void system::unmount()
    {
        return;
    }

    bool system::is_mounted()
    {
        return true;
    }

}}

#endif // JE_PLATFORM_LINUX
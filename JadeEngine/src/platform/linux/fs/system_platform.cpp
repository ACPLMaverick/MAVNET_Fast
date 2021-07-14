#include "fs/system.h"

#if JE_PLATFORM_LINUX

#include <unistd.h>

namespace je { namespace fs {

    static bool s_mounted = false;

    void system::mount()
    {
        if(s_mounted == false)
        {
            s_mounted = (chdir(JE_DATA_DIR) == 0);
        }
    }

    void system::unmount()
    {
        if(s_mounted)
        {
            chdir("..");
            s_mounted = false;
        }
    }

    bool system::is_mounted()
    {
        return s_mounted;
    }

}}

#endif // JE_PLATFORM_LINUX
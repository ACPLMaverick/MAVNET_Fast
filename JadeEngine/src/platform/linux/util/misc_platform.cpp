#include "util/misc.h"

#if JE_PLATFORM_LINUX

#include "global.h"
#include <signal.h>

namespace je { namespace util {

    // Cross-platform implementations.
    data::string misc::call_system_command(const data::string& command)
    {
        FILE* fp;
        char path[1035];

        fp = popen(command.get_data(), "r");
        if(fp == nullptr)
        {
            JE_fail("Failed calling system command [%s].", command.get_data());
            return data::string();
        }

        data::string ret;

        while(fgets(path, sizeof(path), fp) != nullptr)
        {
            ret.append(path);
        }

        pclose(fp);
        return ret;
    }

    void misc::debugbreak()
    {
        raise(SIGTRAP);
    }

}}

#endif
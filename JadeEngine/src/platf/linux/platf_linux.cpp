#include "platf/platf.h"
#include "platf_linux.h"

#if JE_PLATFORM_LINUX

#include "global.h"

namespace je { namespace platf {

namespace util
{
    // Cross-platform implementations.
    data::string get_file_separator()
    {
        return data::string("/");
    }

    data::string call_system_command(const data::string& command)
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
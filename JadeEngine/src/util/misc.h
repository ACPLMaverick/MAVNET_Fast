#pragma once

#include "global.h"

namespace je { namespace util {

    class misc
    {
    public:
        static data::string get_file_separator();
        static data::string call_system_command(const data::string& command);
        static void debugbreak();
        static const char* trim_file_name(const char* absolute_file_name);
    };

}}
#pragma once

// Cross-platform header with declarations of platform-specific thingies.

#include "data/string.h"

namespace je { namespace platf {

    namespace util
    {
        data::string get_file_separator();
        data::string call_system_command(const data::string& command);
        void debugbreak();
    }

}}
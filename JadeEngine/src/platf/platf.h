#pragma once

// Cross-platform header with declarations of platform-specific thingies.

namespace je { namespace platf {

    namespace util
    {
        const char* get_file_separator();
        void debugbreak();
    }

}}
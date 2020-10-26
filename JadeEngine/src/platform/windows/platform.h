#pragma once

#if JE_PLATFORM_WINDOWS

#define NOMINMAX
#include "Windows.h"

namespace je { namespace windows {

    class misc
    {
    public:
        static void print_last_error();
    };

}}

#endif
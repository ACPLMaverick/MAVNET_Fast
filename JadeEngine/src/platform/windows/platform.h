#pragma once

#if JE_PLATFORM_WINDOWS

#include "Windows.h"

namespace je { namespace platform {

    namespace util
    {
        void print_last_error();
    }

}}

#endif
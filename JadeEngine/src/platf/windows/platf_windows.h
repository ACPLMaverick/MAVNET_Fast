#pragma once

#if JE_PLATFORM_WINDOWS

#define NOMINMAX
#include "Windows.h"

namespace je { namespace platf {

    namespace util
    {
        void print_last_error();
    }

}}

#endif
#pragma once

#include "global.h"

namespace je { namespace util {

    class misc
    {
    public:
        // Does not allocate memory, returns pointer within the same string.
        static const char* trim_file_name(const char* absolute_file_name);
    };

}}
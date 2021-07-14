#pragma once

#include "global.h"

#define JE_DATA_DIR "data" "_" JE_NAME_OF_PLATFORM "_" JE_NAME_OF_CONFIG
#define JE_DATA_EXTENSION ".jcd"

namespace je { namespace fs {
    
    // This is a general class for mounting/unmounting entire file system.
    // Also responsible for opening/closing files.
    // This gets implemented in platform-specific source files.
    class system
    {
    public:

        static void mount();
        static void unmount();
        static bool is_mounted();

    };

}}
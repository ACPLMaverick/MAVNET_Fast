#pragma once

#include "global.h"

namespace je { namespace platf {

    class message_box
    {
    public:

        enum class info_level : uint8_t
        {
            k_info = 0,
            k_warning = 1,
            k_error = 2
        };

        JE_bitfield button_flag
        {
            k_continue  = (1 << 0),
            k_cancel    = (1 << 1),
            k_retry     = (1 << 2),
            k_yes       = (1 << 3),
            k_no        = (1 << 4),
            k_all       = 0xFF
        };

        static button_flag show(const char* message, info_level information_level, int buttons, bool is_blocking);
    };

}}
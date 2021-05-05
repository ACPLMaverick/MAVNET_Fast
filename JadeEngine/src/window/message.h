#pragma once

#include "global.h"

namespace je { namespace window {

    enum class message_type : u8
    {
        k_closed,
        k_minimized,
        k_maximized,
        k_resized,
        k_focus_lost,
        k_focus_gained
    };

// TODO make these compiler-independent.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"

    class message
    {
    public:

        message(message_type type, u16 param_a = 0, u16 param_b = 0);
        ~message();

        message_type get_type() const { return m_type; }
        u16 get_param_a() const { return m_param_a; }
        u16 get_param_b() const { return m_param_b; }
        u32 get_param_combined() const { return m_param_combined; }

    protected:

        union
        {
            struct
            {
                u16 m_param_a;
                u16 m_param_b;
            };
            u32 m_param_combined;
        };
        message_type m_type;
    };

#pragma clang diagnostic pop

}}
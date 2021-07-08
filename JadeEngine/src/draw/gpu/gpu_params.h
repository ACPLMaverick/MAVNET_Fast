#pragma once

#include "global.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw {

    struct gpu_params
    {
        i8 m_forced_adapter_index;
    };

    struct presenter_params
    {
        const window::window& m_window;
        u8 m_num_buffers;
        bool m_is_hdr;
        bool m_is_vsync;
    };

}}
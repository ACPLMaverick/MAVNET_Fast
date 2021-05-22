#pragma once

#include "global.h"

namespace je { namespace draw {

    struct gpu_params
    {
        u16 m_backbuffer_width;
        u16 m_backbuffer_height;
        u8 m_num_buffers;
        bool m_is_hdr;
    };

}}
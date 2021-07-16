#pragma once

#include "global.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw { namespace gpu {

    enum class shader_stage : u8;

    struct device_params
    {
        i8 m_forced_adapter_index;
    };

    struct presenter_params
    {
        const window::window& m_window;
        u8 m_num_buffers;
        bool m_is_vsync;
        bool m_is_hdr;
    };

    struct shader_params
    {
        const data::string& m_file_name;
        shader_stage m_stage;
    };

}}}
#pragma once

#include "global.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw { namespace gpu {

    // Data types/formats.

    enum class shader_stage : u8
    {
        k_vertex,
        k_tess_ctrl,
        k_tess_eval,
        k_geometry,
        k_fragment,
        k_compute,
        k_rt_gen,   // RT Generation
        k_rt_int,   // RT Intersection
        k_rt_chit,  // RT Closest Hit
        k_rt_ahit,  // RT Any Hit
        k_rt_miss,  // RT Miss
        k_enum_size,
    };

    class vertex_semantic
    {
    public:
        using int_type = u8;

        enum
        {
            k_none = 0,
            k_position = (1 << 0),
            k_color = (1 << 1),
            k_uv = (1 << 2),
            k_normal = (1 << 3),
            k_binormal = (1 << 4),
            k_tangent = (1 << 5),
            k_skin_weight = (1 << 6),
            k_skin_index = (1 << 7),
            k_enum_flag_last = k_skin_index
        };

        operator int_type() const
        {
            return m_value;
        }

        static constexpr size get_num_components(const vertex_semantic sem);
        static constexpr size get_component_num_bytes(const vertex_semantic sem);
        static constexpr size get_num_bytes(const vertex_semantic sem);
        static size calculate_num_bytes(const vertex_semantic sem);

        int_type m_value;
    };

    enum class texture_format : u8
    {
        k_bgra8,
        k_b10g11r11,
        k_bgra16,
        k_rg16,
        k_r32,
        k_r16,
        k_r8,
        k_d24s8,
        k_d32,
        k_d16,
        // TODO More to come up in the future.
        k_enum_size 
    };

    extern bool is_texture_format_depth(texture_format format);
    extern bool is_texture_format_depth_stencil(texture_format format);

    // ////////////////////

    // Initialization structs.

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

    // ////////////////////

}}}
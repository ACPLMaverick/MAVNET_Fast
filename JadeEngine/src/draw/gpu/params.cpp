#include "params.h"

namespace je { namespace draw { namespace gpu {

    constexpr size vertex_semantic::get_num_components(const vertex_semantic a_sem)
    {
        switch(a_sem.m_value)
        {
        case vertex_semantic::k_position:
            return 3;
        case vertex_semantic::k_color:
            return 4;
        case vertex_semantic::k_uv:
            return 2;
        case vertex_semantic::k_normal: // TODO These three can later be 2...
            return 3;
        case vertex_semantic::k_binormal:
            return 3;
        case vertex_semantic::k_tangent:
            return 3;
        case vertex_semantic::k_skin_weight:
            return 1;
        case vertex_semantic::k_skin_index:
            return 1;
        default:
            JE_fail("Not supported semantic type.");
            return 0;
        }
    }

    constexpr size vertex_semantic::get_component_num_bytes(const vertex_semantic a_sem)
    {
        switch(a_sem.m_value)
        {
        case vertex_semantic::k_position:
        case vertex_semantic::k_uv:
        case vertex_semantic::k_normal:
        case vertex_semantic::k_binormal:
        case vertex_semantic::k_tangent:
        case vertex_semantic::k_skin_weight:
            return sizeof(u32);
        case vertex_semantic::k_color:
        case vertex_semantic::k_skin_index:
            return sizeof(u8);
        default:
            JE_fail("Not supported semantic type.");
            return 0;
        }
    }

    constexpr size vertex_semantic::get_num_bytes(const vertex_semantic a_sem)
    {
        return vertex_semantic::get_num_components(a_sem) * vertex_semantic::get_component_num_bytes(a_sem);
    }

    size vertex_semantic::calculate_num_bytes(const vertex_semantic a_sem)
    {
        size num_bytes = 0;
        vertex_semantic::int_type check_value = vertex_semantic::k_none;
        do
        {
            if(a_sem & check_value)
            {
                num_bytes += get_component_num_bytes({check_value});
            }
            check_value <<= 1;
        }
        while(check_value != vertex_semantic::k_enum_flag_last);
        return num_bytes;
    }

    bool texture_format::is_depth() const
    {
        switch (m_value)
        {      
        case texture_format::k_d24s8:
        case texture_format::k_d32:
        case texture_format::k_d16: 
            return true;
        default:
            return false;
        }
    }

    bool texture_format::is_depth_stencil() const
    {
        switch (m_value)
        {      
        case texture_format::k_d24s8:
            return true;
        default:
            return false;
        }
    }

    size texture_format::get_pixel_num_bytes() const
    {
        switch (m_value)
        {
        case texture_format::k_bgra8:
            return 4;
        case texture_format::k_bgr8:
            return 3;
        case texture_format::k_b10g11r11:
            return 4;
        case texture_format::k_bgra16:
            return 8;
        case texture_format::k_rg16:
            return 4;
        case texture_format::k_r32:
            return 4;
        case texture_format::k_r16:
            return 2;
        case texture_format::k_r8:
            return 1;
        case texture_format::k_d24s8:
            return 4;
        case texture_format::k_d32:
            return 4;
        case texture_format::k_d16:
            return 2;
        default:
            JE_fail("Not specified texture format.");
            return 0;
        }
    }

}}}
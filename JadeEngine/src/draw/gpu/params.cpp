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

    bool is_texture_format_depth(texture_format a_format)
    {
        switch (a_format)
        {      
        case texture_format::k_d24s8:
        case texture_format::k_d32:
        case texture_format::k_d16: 
            return true;
        default:
            return false;
        }
    }

    bool is_texture_format_depth_stencil(texture_format a_format)
    {
        switch (a_format)
        {      
        case texture_format::k_d24s8:
            return true;
        default:
            return false;
        }
    }

}}}
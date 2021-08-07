#include "texture.h"

namespace je { namespace draw { namespace gpu {

    texture::texture(const texture_params& a_params)
        : m_size(a_params.m_size)
        , m_format(a_params.m_format)
    {
    }

    texture::texture(math::screen_size a_size, texture_format a_format)
        : m_size(a_size)
        , m_format(a_format)
    {
    }

    texture::~texture()
    {
    }

    bool texture::init(device& a_device, const texture_params& a_params)
    {
        if(m_size.x == 0 || m_size.y == 0)
        {
            JE_fail("Bad texture size specified.");
            return false;
        }

        JE_verify_bailout(init_internal(a_device, a_params), false, "Failed to init texture resources.");

        if(a_params.m_buffer_ptr != nullptr && a_params.m_buffer_ptr->size() > 0)
        {
            JE_verify_bailout(fill_with_buffer_data_internal(a_device, *a_params.m_buffer_ptr), false, "Failed to fill buffer with data.");
        }
        else if(a_params.m_is_cleared)
        {
            JE_verify_bailout(clear_internal(a_device, a_params.m_clear_color), false, "Failed to init-clear texture");
        }
        
        return true;
    }

    u8 texture::compute_num_mips_from_size() const
    {
        u16 width = m_size.x;
        u8 num_mips = 1;
        while(width > 2) // So 4x4 would be the smallest mip level.
        {
            width /= 2;
            ++num_mips;
        }
        return num_mips;
    }

}}}
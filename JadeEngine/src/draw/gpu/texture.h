#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "params.h"
#include "math/ivec.h"
#include "math/color.h"

namespace je { namespace draw { namespace gpu {

    class factory;
    class device;
    class presenter;

    // If there is a buffer ptr specified, data will be loaded from that buffer.
    // If there's no buffer ptr specified, texture will be cleared with clear color
    // but only if m_is_cleared is set to true.
    struct texture_params
    {
        const data::array<u8>* m_buffer_ptr = nullptr;
        math::screen_size m_size = math::screen_size(0);
        math::color m_clear_color = math::color();
        texture_format m_format = texture_format::k_bgra8;
        bool m_is_mipmapped;
        bool m_is_cleared;
    };

    class texture : public mem::allocatable
    {
    public:

        math::screen_size get_size() const { return m_size; }
        texture_format get_format() const { return m_format; }
        
    protected:

        texture(const texture_params& a_params);
        texture(math::screen_size size, texture_format format);
        virtual ~texture();

        bool init(device& a_device, const texture_params& params);
        virtual void shutdown(device& a_device) = 0;
        virtual bool init_internal(device& a_device, const texture_params& params) = 0;
        virtual bool clear_internal(device& a_device, math::color clear_color) = 0;
        virtual bool fill_with_buffer_data_internal(device& a_device, const data::array<u8>& buffer) = 0;

        size get_data_num_bytes() const { return m_size.x * m_size.y * m_format.get_pixel_num_bytes(); }

    protected:

        u8 compute_num_mips_from_size() const;

        math::screen_size m_size;
        texture_format m_format;

        friend class factory;
        friend class presenter; // Presenter can also create texture objects from back buffer.
    };

}}}
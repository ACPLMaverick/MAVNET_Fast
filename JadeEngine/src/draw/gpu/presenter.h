#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "math/ivec.h"
#include "params.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw { namespace gpu {

    class device;
    class texture;

    // This class encapsulates whole swapchain and presentation functionality and synchronization.
    class presenter : public mem::allocatable_persistent
    {
    public:

        enum class capabilities : u32
        {
            k_none = 0,
            k_triple_buffering = (1 << 0),
            k_vsync = (1 << 1),
            k_immediate = (1 << 2),
            k_hdr = (1 << 3)
        };

    public:

        virtual bool present(device& a_device) = 0;
        virtual bool set_vsync(bool is_vsync) = 0;
        virtual bool set_hdr(bool is_hdr) = 0;
        virtual bool recreate(device& a_device, const window::window& updated_window);

        texture& get_current_buffer() const { return *m_buffers[m_current_buffer]; }
        u8 get_num_buffers() const { return m_buffers.size(); }
        void get_all_buffers(data::array<texture*>& out_buffers) const { out_buffers = m_buffers; }

        bool has_capabilities(capabilities a_caps) const { return (static_cast<u32>(m_capabilities) & static_cast<u32>(a_caps)) == static_cast<u32>(a_caps); }
        math::screen_size get_dimensions() const { return m_backbuffer_dims; }
        bool is_vsync() const { return m_is_vsync; }
        bool is_hdr() const { return m_is_hdr; }

    protected:

        presenter(const presenter_params& params);
        virtual ~presenter() {}

        virtual bool init(device& a_device, const presenter_params& params) = 0;
        virtual void shutdown(device& a_device) = 0;

    protected:

        data::array<texture*> m_buffers;
        capabilities m_capabilities;
        math::screen_size m_backbuffer_dims;
        u8 m_current_buffer;
        bool m_is_vsync;
        bool m_is_hdr;

    protected:

        friend class factory;
    };
    
}}}
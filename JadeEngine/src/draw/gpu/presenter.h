#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "params.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw { namespace gpu {

    class dev;

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

        virtual ~presenter() {}

        virtual void shutdown(dev& a_dev) = 0;
        virtual bool present(dev& a_dev/*TODO params : presented render target. Window position offset.*/) = 0;
        virtual bool set_vsync(bool is_vsync) = 0;
        virtual bool set_hdr(bool is_hdr) = 0;
        virtual bool recreate(dev& a_dev, const window::window& updated_window);

        bool has_capabilities(capabilities a_caps) const { return (static_cast<u32>(m_capabilities) & static_cast<u32>(a_caps)) == static_cast<u32>(a_caps); }
        u16 get_width() const { return m_backbuffer_width; }
        u16 get_height() const { return m_backbuffer_height; }
        u16 get_num_buffers() const { return m_num_buffers; }
        bool is_vsync() const { return m_is_vsync; }
        bool is_hdr() const { return m_is_hdr; }

    protected:

        presenter(const presenter_params& params);

        virtual bool init(dev& a_dev, const presenter_params& params) = 0;

    protected:

        capabilities m_capabilities;
        u16 m_backbuffer_width;
        u16 m_backbuffer_height;
        u8 m_num_buffers;
        bool m_is_vsync;
        bool m_is_hdr;
    };
    
}}}
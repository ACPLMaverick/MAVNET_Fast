#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "gpu_params.h"

namespace je { namespace window {
    class window;
}}

namespace je { namespace draw {

    class gpu;

    // This class encapsulates whole swapchain and presentation functionality and synchronization.
    class presenter : public mem::allocatable_persistent
    {
    public:

        enum class capabilities : u32
        {
            k_none = 0,
            k_triple_buffering = (1 << 0),
            k_vsync = (1 << 1),
            k_hdr = (1 << 2)
        };

    public:

        virtual ~presenter() {}

        virtual void shutdown(gpu& a_gpu) = 0;
        virtual bool present(gpu& a_gpu/*TODO params : presented render target. Window position offset.*/) = 0;
        virtual bool set_vsync(bool is_vsync) = 0;
        virtual bool set_hdr(bool is_hdr) = 0;
        virtual bool recreate(gpu& a_gpu, const window::window& updated_window);

        bool has_capabilities(capabilities a_caps) const { return (static_cast<u32>(m_capabilities) & static_cast<u32>(a_caps)) == static_cast<u32>(a_caps); }
        u16 get_width() const { return m_backbuffer_width; }
        u16 get_height() const { return m_backbuffer_height; }
        u16 get_num_buffers() const { return m_num_buffers; }
        bool is_vsync() const { return m_is_vsync; }
        bool is_hdr() const { return m_is_hdr; }

    protected:

        presenter(const presenter_params& params);

        virtual bool init(gpu& a_gpu, const window::window& a_window) = 0;

    protected:

        capabilities m_capabilities;
        u16 m_backbuffer_width;
        u16 m_backbuffer_height;
        u8 m_num_buffers;
        bool m_is_vsync;
        bool m_is_hdr;
    };
    
}}
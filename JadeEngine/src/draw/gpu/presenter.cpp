#include "presenter.h"
#include "window/window.h"

namespace je { namespace draw { namespace gpu {

    presenter::presenter(const presenter_params& a_params)
        : m_capabilities(capabilities::k_none)
        , m_backbuffer_width(a_params.m_window.get_width())
        , m_backbuffer_height(a_params.m_window.get_height())
        , m_num_buffers(a_params.m_num_buffers)
        , m_is_vsync(a_params.m_is_vsync)
        , m_is_hdr(a_params.m_is_hdr)
    {
    }

    bool presenter::recreate(device& a_device, const window::window& a_updated_window)
    {
        shutdown(a_device);
        return init(a_device, { a_updated_window, m_num_buffers, m_is_vsync, m_is_hdr });
    }

}}}
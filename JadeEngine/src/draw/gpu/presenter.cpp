#include "presenter.h"
#include "window/window.h"

namespace je { namespace draw {

    presenter::presenter(const presenter_params& a_params)
        : m_capabilities(capabilities::k_none)
        , m_backbuffer_width(a_params.m_window.get_width())
        , m_backbuffer_height(a_params.m_window.get_height())
        , m_num_buffers(a_params.m_num_buffers)
        , m_is_vsync(a_params.m_is_vsync)
        , m_is_hdr(a_params.m_is_hdr)
    {
    }

    bool presenter::recreate(gpu& a_gpu, const window::window& a_updated_window)
    {
        shutdown(a_gpu);
        return init(a_gpu, a_updated_window);
    }

}}
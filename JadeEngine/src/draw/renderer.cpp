#include "renderer.h"
#include "window/window.h"
#include "gpu/dev.h"
#include "gpu/presenter.h"

namespace je { namespace draw {

    renderer::renderer(window::window& win)
        : m_window(win)
        , m_dev(gpu::dev::create_dev({ -1 }))    // TODO more sophisticated way of creating this.
        , m_presenter(m_dev->create_presenter({ m_window, 3, false, false }))
    {
    }

    renderer::~renderer()
    {
        JE_safe_delete(m_presenter, m_presenter->shutdown(*m_dev));
        gpu::dev::destroy_dev(m_dev);
        m_dev = nullptr;
    }

    void renderer::draw()
    {

    }

}}
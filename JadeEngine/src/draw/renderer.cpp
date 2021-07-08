#include "renderer.h"
#include "window/window.h"
#include "gpu/gpu.h"
#include "gpu/presenter.h"

namespace je { namespace draw {

    renderer::renderer(window::window& win)
        : m_window(win)
        , m_gpu(gpu::gpu::create_gpu({ -1 }))    // TODO more sophisticated way of creating this.
        , m_presenter(m_gpu->create_presenter({ m_window, 3, false, false }))
    {
    }

    renderer::~renderer()
    {
        JE_safe_delete(m_presenter, m_presenter->shutdown(*m_gpu));
        gpu::destroy_gpu(m_gpu);
        m_gpu = nullptr;
    }

    void renderer::draw()
    {

    }

}}
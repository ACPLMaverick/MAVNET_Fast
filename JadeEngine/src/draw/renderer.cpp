#include "renderer.h"
#include "window/window.h"
#include "gpu/gpu.h"

namespace je { namespace draw {

    renderer::renderer(window::window& win)
        : m_window(win)
        , m_gpu(gpu::gpu::create_gpu({ m_window.get_width(), m_window.get_height(), 3, false }))    // TODO more sophisticated way of creating this.
    {
    }

    renderer::~renderer()
    {

    }

    void renderer::draw()
    {

    }

}}
#include "renderer.h"
#include "window/window.h"
#include "gpu/factory.h"
#include "gpu/device.h"
#include "gpu/presenter.h"

namespace je { namespace draw {

    renderer::renderer(window::window& win)
        : m_window(win)
        , m_device(gpu::factory::init({ -1 }))    // TODO more sophisticated way of creating this.
        , m_presenter(gpu::factory::get().create_presenter({ m_window, 3, false, false }))
    {
    }

    renderer::~renderer()
    {
        gpu::factory::get().destroy(m_presenter);
        gpu::factory::destroy();
        m_device = nullptr; // We don't own the device, factory does.
    }

    void renderer::draw()
    {

    }

}}
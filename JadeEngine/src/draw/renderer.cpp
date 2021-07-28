#include "renderer.h"
#include "window/window.h"
#include "gpu/factory.h"
#include "gpu/device.h"
#include "gpu/presenter.h"
#include "gpu/pass.h"
#include "gpu/pso.h"

namespace je { namespace draw {

    renderer::renderer(window::window& win)
        : m_window(win)
        , m_device(gpu::factory::init({ -1 }))    // TODO more sophisticated way of creating this.
        , m_presenter(gpu::factory::get().create_presenter({ m_window, 3, false, false }))
        , m_single_pass(create_single_pass())
        , m_single_pso(create_single_pso())
    {
    }

    renderer::~renderer()
    {
        gpu::factory::get().destroy(m_single_pso);
        gpu::factory::get().destroy(m_single_pass);
        gpu::factory::get().destroy(m_presenter);
        gpu::factory::destroy();
        m_device = nullptr; // We don't own the device, factory does.
    }

    void renderer::draw()
    {

    }

    gpu::pass* renderer::create_single_pass()
    {
        gpu::pass_params params;
        params.m_render_target_infos.push(gpu::texture_format::k_bgra8, true, true);
        gpu::pass_operation* op = params.m_ops.push_uninitialized();
        op->m_output_render_target_indices.push(0);
        return gpu::factory::get().create_pass(params);
    }

    gpu::pso* renderer::create_single_pso()
    {
        JE_todo();
        return nullptr;
    }

}}
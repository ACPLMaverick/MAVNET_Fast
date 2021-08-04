#include "pso.h"

namespace je { namespace draw { namespace gpu {

    render_state::dynamic_data::dynamic_data()
        : m_shaders(static_cast<shader*>(nullptr))
        , m_push_constants_buffer()
    {
    }

    void render_state::dynamic_data::bind_shader(shader* a_shader)
    {
        JE_assert_bailout_noret(a_shader != nullptr, "Bound shader is not valid.");
        JE_assert(m_shaders[static_cast<size>(a_shader->get_stage())] == nullptr, "Overwriting an existing shader. This shouldn't normally happen.");
        m_shaders[static_cast<size>(a_shader->get_stage())] = a_shader;
    }

    void render_state::dynamic_data::clear_shader(shader_stage a_stage)
    {
        m_shaders[static_cast<size>(a_stage)] = nullptr;
    }

    render_state::static_data::static_data()
        : m_viewport_perc_offset(100, 100, 0, 0)
        , m_scissor_perc_offset(100, 100, 0, 0)
        , m_stencil_value(0)
        , m_multisample_count(multisample_count::k_none)
        , m_multisample_mode(multisample_mode_flag::k_disabled)
        , m_depth_test_mode(depth_test_mode::k_less_equal)
        , m_stencil_mode(stencil_mode::k_none)
        , m_blend_mode(blend_mode::k_overwrite)
        , m_depth_write(true)
        , m_shadow_depth_bias_and_clamp(false)
        , m_cull_backface(true)
    {
    }

    render_state::render_state()
        : m_dynamic()
        , m_static()
    {
    }
        
    bool render_state::has_valid_shader_stage(shader_stage a_stage) const
    {
        return m_dynamic.m_shaders[static_cast<size>(a_stage)] != nullptr;
    }

    bool render_state::has_valid_push_constants_buffer() const
    {
        return m_dynamic.m_push_constants_buffer.size() > 0;
    }

    bool render_state::has_valid_viewport() const
    {
        return m_static.m_viewport_perc_offset.x > 0
            && m_static.m_viewport_perc_offset.y > 0;
    }

    bool render_state::has_valid_scissor() const
    {
        return m_static.m_scissor_perc_offset.x > 0
            && m_static.m_scissor_perc_offset.y > 0;
    }

    bool render_state::has_valid_multisampling() const
    {
        return m_static.m_multisample_count != multisample_count::k_none
            && m_static.m_multisample_mode != multisample_mode_flag::k_disabled;
    }

    pso_params::pso_params(const pass& a_pass, u8 a_operation_idx, math::screen_size a_source_screen_size)
        : m_render_state()
        , m_pass(a_pass)
        , m_base_pso(nullptr)
        , m_source_screen_size(a_source_screen_size)
        , m_operation_idx(a_operation_idx)
    {
    }

    pso_hash::pso_hash(const render_state& rs)
        : m_hash(0)
    {
        m_hash = util::struct_hash<render_state::static_data>(rs.m_static).get_value();
    }

    pso::pso(const pso_params& a_params)
        : m_state(a_params.m_render_state)
        , m_hash(m_state)
    {
    }

    pso::~pso()
    {

    }

    math::screen_size pso::compute_screen_size(math::screen_size source, math::screen_rect perc)
    {
        JE_assert(perc.x <= 100);
        JE_assert(perc.y <= 100);
        if(perc.x < 100)
        {
            source.x = static_cast<u16>(static_cast<float>(source.x) * static_cast<float>(perc.x) * 0.01f);
        }
        if(perc.y < 100)
        {
            source.y = static_cast<u16>(static_cast<float>(source.y) * static_cast<float>(perc.y) * 0.01f);
        }
        return source;
    }

}}}
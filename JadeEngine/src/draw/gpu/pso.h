#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "shader.h"
#include "math/ivec.h"
#include "util/struct_hash.h"

namespace je { namespace draw { namespace gpu {

    class device;
    class pass;

    enum class depth_test_mode : u8
    {
        k_none,
        k_less_equal,
        k_equal
    };

    enum class stencil_mode : u8
    {
        k_none,
        k_write,
        k_test
    };

    enum class blend_mode : u8
    {
        k_overwrite,
        k_alpha_based,
        k_additive
    };

    enum class multisample_mode_flag : u8
    {
        k_disabled = 0,
        k_enabled = (1 << 0),
        k_alpha_to_coverage = (1 << 1),
        k_alpha_to_one = (1 << 2)
    };

    enum class multisample_count : u8
    {
        k_none = 0,
        k_2 = (1 << 0),
        k_4 = (1 << 1),
        k_8 = (1 << 2),
        k_16 = (1 << 3)
    };

    class render_state
    {
    public:

        struct dynamic_data
        {
            using shaders = data::static_array<shader*, static_cast<size>(shader_stage::k_enum_size)>;

            dynamic_data();
            void bind_shader(shader* a_shader);
            void clear_shader(shader_stage a_stage);

            shaders m_shaders;
            data::array<u8> m_push_constants_buffer;
        };

        struct static_data
        {
            static_data();

            // Viewport and scissor perc is stored as an integer value of (0, 100).
            // Viewport and scissor offset is stored in pixels.
            math::screen_rect m_viewport_perc_offset;
            math::screen_rect m_scissor_perc_offset;
            u8 m_stencil_value;
            multisample_count m_multisample_count : 4;
            multisample_mode_flag m_multisample_mode : 3;
            depth_test_mode m_depth_test_mode : 2;
            stencil_mode m_stencil_mode : 2;
            blend_mode m_blend_mode : 2;
            bool m_depth_write : 1;
            bool m_shadow_depth_bias_and_clamp : 1;
            bool m_cull_backface : 1;
            // Fixed values (for now):
            // - topology (always triangle list)
            // - polygon fill mode (always fill - wireframe will use shader)
            // - front face (always clockwise)
            // - Blend op per attachment (will probably be changed)
        };

        // Creates default render state without dynamic data assigned and viewport/scissor specified.
        render_state();
        
        bool has_valid_shader_stage(shader_stage stage) const;
        bool has_valid_push_constants_buffer() const;
        bool has_valid_viewport() const;
        bool has_valid_scissor() const;
        bool has_valid_multisampling() const;

    public:

        dynamic_data m_dynamic;
        static_data m_static;
    };

    struct pso_params
    {
        pso_params(const pass& a_pass, u8 a_operation_idx, math::screen_size a_source_screen_size);

        render_state m_render_state;
        const pass& m_pass;
        class pso* m_base_pso;
        math::screen_size m_source_screen_size;
        u8 m_operation_idx;
    };

    class pso_hash
    {
    public:

        pso_hash(const render_state& rs);
        u64 get_value() const { return m_hash; }
        operator u64() const { return get_value(); }

    private:

        u64 m_hash;
    };
    
    class pso : public mem::allocatable
    {
    public:

        bool operator==(const pso& a_other) const { return m_hash == a_other.m_hash; }
        bool operator!=(const pso& a_other) const { return m_hash != a_other.m_hash; }

        const render_state& get_render_state() const { return m_state; }
        pso_hash get_hash() const { return m_hash; }

        virtual bool update_for_new_size(math::screen_size new_size) = 0; 

    protected:

        pso(const pso_params& params);
        virtual ~pso();

        virtual bool init(device& a_device, const pso_params& params) = 0;
        virtual void shutdown(device& a_device) = 0;

        static math::screen_size compute_screen_size(math::screen_size source, math::screen_rect perc);

        render_state m_state;
        pso_hash m_hash;

        friend class factory;
    };

}}}
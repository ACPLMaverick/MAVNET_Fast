#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "params.h"
#include "util/struct_hash.h"

namespace je { namespace draw { namespace gpu {

    struct pass_operation
    {
        using inputs = data::static_stack<u32, 8>;
        using outputs = data::static_stack<u32, 8>;
        inputs m_input_render_target_indices;
        outputs m_output_render_target_indices;
    };

    struct render_target_info
    {
        render_target_info(
                texture_format a_format = texture_format::k_bgra8,
                bool a_is_cleared = false,
                bool a_is_present_surface = false)
            : m_format(a_format)
            , m_is_cleared(a_is_cleared)
            , m_is_present_surface(a_is_present_surface)
        {
        }

        texture_format::int_type m_format : 6;
        bool m_is_cleared : 1;
        bool m_is_present_surface : 1;
    };

    static_assert(static_cast<u8>(texture_format::k_enum_size) < (1 << 6), "Too many bytes used for texture_formats. Cannot fit in rt_info.");

    struct pass_params
    {
        using render_target_infos = data::static_stack<render_target_info, 16>;
        using operations = data::static_stack<pass_operation, 8>;
        render_target_infos m_render_target_infos;
        operations m_ops;
    };

    class device;
    class cmd;

    using pass_hash = util::struct_hash<pass_params>;

    // Pass is an array of sequential operation on render targets.
    // We specify what texture format will be written to in each operation.
    // In RenderPass there are:
    // - attachments
    // - subpasses (input attachments, output-color attachments, output-depth attachment)
    // - dependencies between subpasses (these can be deduced from subpasses)
    class pass : public mem::allocatable
    {
    public:

        const pass_params::render_target_infos& get_render_target_infos() const { return m_params.m_render_target_infos; }
        const pass_params::operations& get_operations() const { return m_params.m_ops; }
        pass_hash get_hash() const { return m_hash; }

        void set(cmd& a_cmd);
        void unset(cmd& a_cmd);

    protected:

        pass(const pass_params& params);
        virtual ~pass();

        virtual bool init(device& a_device, const pass_params& params) = 0;
        virtual void shutdown(device& a_device) = 0;

    protected:

        pass_params m_params;
        pass_hash m_hash;

    protected:

        friend class factory;
    };

    // RIIA wrapper over pass set/unset.
    // TODO this will be probably done later for enclosing RenderPass, but for now let's have it here.
    class pass_scope
    {
    public:

        pass_scope(cmd& a_cmd, pass& a_pass)
            : m_cmd(a_cmd)
            , m_pass(a_pass)
        {
            m_pass.set(m_cmd);
        }

        ~pass_scope()
        {
            m_pass.unset(m_cmd);
        }

    private:

        cmd& m_cmd;
        pass& m_pass;
    };

}}}
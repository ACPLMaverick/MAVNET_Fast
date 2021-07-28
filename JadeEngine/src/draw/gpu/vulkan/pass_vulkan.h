#pragma once

#include "draw/gpu/pass.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class pass_vulkan : public pass
    {
    public:

        VkRenderPass get_render_pass() const { return m_render_pass; }
        
    protected:

        pass_vulkan(const pass_params& params);
        ~pass_vulkan();

        bool init(device& a_device, const pass_params& params) override;
        void shutdown(device& a_device) override;

    protected:

        static VkImageLayout get_layout_for_rt_info(const render_target_info& info, bool is_input);

    protected:

        VkRenderPass m_render_pass;

    protected:

        friend class factory;
    };

}}}
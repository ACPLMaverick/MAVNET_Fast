#pragma once

#include "draw/gpu/pso.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class pso_vulkan : public pso
    {
    public:

        bool update_for_new_size(math::screen_size new_size) override; 

    protected:

        pso_vulkan(const pso_params& params);
        ~pso_vulkan();

        bool init(device& a_device, const pso_params& params) override;
        void shutdown(device& a_device) override;

    protected:

        VkPipelineLayout m_layout;
        VkPipeline m_pipe;

        friend class factory;
    };

}}}
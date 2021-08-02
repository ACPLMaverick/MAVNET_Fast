#pragma once

#include "draw/gpu/shader.h"
#include "global_vulkan.h"

namespace je { namespace draw { namespace gpu {

    class shader_vulkan : public shader
    {
    public:

        VkShaderModule get_module() const { return m_module; }
        
    protected:

        shader_vulkan(const shader_params& a_params);
        ~shader_vulkan();

        void shutdown(device& a_device) override;
        bool init_module(device& a_device, const data::array<u8>& data) override;

    protected:

        VkShaderModule m_module;

    protected:

        friend class factory;

    };

}}}
#pragma once

#include "draw/gpu/gpu.h"

namespace je { namespace draw {

    class gpu_vulkan : public gpu
    {
    public:

        ~gpu_vulkan();

    protected:

        gpu_vulkan();

        bool init(const gpu_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "Vulkan"; }

        friend class gpu;
    };

}}
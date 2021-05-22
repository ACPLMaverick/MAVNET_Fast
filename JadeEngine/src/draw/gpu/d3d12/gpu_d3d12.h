#pragma once

#include "draw/gpu/gpu.h"

namespace je { namespace draw {

    class gpu_d3d12 : public gpu
    {
    public:

        ~gpu_d3d12();

    protected:

        gpu_d3d12();

        bool init(const gpu_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "Direct3D 12"; }

        friend class gpu;
    };

}}
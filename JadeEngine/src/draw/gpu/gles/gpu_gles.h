#pragma once

#include "draw/gpu/gpu.h"

namespace je { namespace draw {

    class gpu_gles : public gpu
    {
    public:

        ~gpu_gles();

    protected:

        gpu_gles();

        bool init(const gpu_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "OpenGL ES"; }

        friend class gpu;
    };

}}
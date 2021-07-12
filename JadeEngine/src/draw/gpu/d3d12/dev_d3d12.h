#pragma once

#include "draw/gpu/dev.h"

namespace je { namespace draw { namespace gpu {

    class dev_d3d12 : public dev
    {
    public:

        ~dev_d3d12();

    protected:

        dev_d3d12();

        bool init(const dev_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "Direct3D 12"; }

        friend class dev;
    };

}}
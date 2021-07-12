#pragma once

#include "draw/gpu/device.h"

namespace je { namespace draw { namespace gpu {

    class device_d3d12 : public device
    {
    public:

    protected:

        device_d3d12();
        ~device_d3d12();

        bool init(const device_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "Direct3D 12"; }

        friend class factory;
    };

}}
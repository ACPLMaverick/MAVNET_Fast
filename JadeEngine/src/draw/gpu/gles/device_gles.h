#pragma once

#include "draw/gpu/device.h"

namespace je { namespace draw { namespace gpu {

    class device_gles : public device
    {
    public:

    protected:

        device_gles();
        ~device_gles();

        bool init(const device_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "OpenGL ES"; }

        friend class factory;
    };

}}}
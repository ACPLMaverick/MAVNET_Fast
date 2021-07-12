#pragma once

#include "draw/gpu/dev.h"

namespace je { namespace draw { namespace gpu {

    class dev_gles : public dev
    {
    public:

        ~dev_gles();

    protected:

        dev_gles();

        bool init(const dev_params& initializer) override;
        bool shutdown() override;
        const char* get_name() override { return "OpenGL ES"; }

        friend class dev;
    };

}}}
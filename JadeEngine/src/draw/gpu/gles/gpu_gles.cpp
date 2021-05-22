#include "gpu_gles.h"

namespace je { namespace draw {

    gpu_gles::~gpu_gles()
    {

    }

    gpu_gles::gpu_gles()
        : gpu()
    {

    }

    bool gpu_gles::init(const gpu_params& a_initializer)
    {
        return false;
    }

    bool gpu_gles::shutdown()
    {
        return true;
    }

}}
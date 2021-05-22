#include "gpu_d3d12.h"

namespace je { namespace draw {

    gpu_d3d12::~gpu_d3d12()
    {

    }

    gpu_d3d12::gpu_d3d12()
        : gpu()
    {

    }

    bool gpu_d3d12::init(const gpu_params& a_initializer)
    {
        return false;
    }

    bool gpu_d3d12::shutdown()
    {
        return true;
    }

}}
#include "gpu_vulkan.h"

namespace je { namespace draw {

    gpu_vulkan::~gpu_vulkan()
    {

    }

    gpu_vulkan::gpu_vulkan()
        : gpu()
    {

    }

    bool gpu_vulkan::init(const gpu_params& a_initializer)
    {
        return false;
    }

    bool gpu_vulkan::shutdown()
    {
        return true;
    }

}}
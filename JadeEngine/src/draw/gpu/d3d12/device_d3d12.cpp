#include "device_d3d12.h"

namespace je { namespace draw { namespace gpu {

    device_d3d12::device_d3d12()
        : gpu()
    {

    }

    device_d3d12::~device_d3d12()
    {

    }

    bool device_d3d12::init(const device_params& a_initializer)
    {
        return false;
    }

    bool device_d3d12::shutdown()
    {
        return true;
    }

}}}
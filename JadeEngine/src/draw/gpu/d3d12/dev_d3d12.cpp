#include "dev_d3d12.h"

namespace je { namespace draw { namespace gpu {

    dev_d3d12::~dev_d3d12()
    {

    }

    dev_d3d12::dev_d3d12()
        : gpu()
    {

    }

    bool dev_d3d12::init(const dev_params& a_initializer)
    {
        return false;
    }

    bool dev_d3d12::shutdown()
    {
        return true;
    }

}}}
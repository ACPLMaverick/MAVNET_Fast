#include "device_gles.h"

namespace je { namespace draw { namespace gpu {

    device_gles::device_gles()
        : device()
    {

    }

    device_gles::~device_gles()
    {

    }

    bool device_gles::init(const device_params& a_initializer)
    {
        return false;
    }

    bool device_gles::shutdown()
    {
        return true;
    }

}}}
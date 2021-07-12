#include "dev_gles.h"

namespace je { namespace draw { namespace gpu {

    dev_gles::~dev_gles()
    {

    }

    dev_gles::dev_gles()
        : dev()
    {

    }

    bool dev_gles::init(const dev_params& a_initializer)
    {
        return false;
    }

    bool dev_gles::shutdown()
    {
        return true;
    }

}}}
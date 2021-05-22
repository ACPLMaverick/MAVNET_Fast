#pragma once

#include "global.h"
#include "gpu_params.h"
#include "mem/allocatable.h"

namespace je { namespace draw {

    class gpu : public mem::allocatable_persistent
    {
    public:

        static gpu* create_gpu(const gpu_params& initializer);
        virtual ~gpu() {}

    protected:

        gpu() {}
        
        virtual bool init(const gpu_params& initializer) = 0;
        virtual bool shutdown() = 0;

        virtual const char* get_name() = 0;

    };

}}
#pragma once

#include "global.h"

namespace je {

    namespace mem
    {
        class mem_manager;
    }

    class engine
    {
    public:

        static mem::mem_manager& get_mem_manager();

    };

}
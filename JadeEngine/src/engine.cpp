#include "engine.h"

#include "mem/mem_manager.h"

namespace je {

    mem::mem_manager& engine::get_mem_manager()
    {
        static mem::mem_manager manager;
        return manager;
    }

}
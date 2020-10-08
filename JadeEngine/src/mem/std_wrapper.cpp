#include "global.h"

#include "std_wrapper.h"

#if JE_DATA_STRUCTS_STD_BACKEND

#include "engine.h"
#include "mem/mem_manager.h"

namespace je { namespace mem {

    void* std_wrapper_allocator_helper::allocate_internal(size_t a_num_bytes, size_t a_alignment)
    {
        return engine::get_mem_manager()
            .allocate<std_wrapper_allocator_helper>(a_num_bytes, static_cast<je::mem::alignment>(a_alignment));
    }

    void std_wrapper_allocator_helper::free_internal(void* a_memory)
    {
        engine::get_mem_manager().free<std_wrapper_allocator_helper>(a_memory);
    }

}}

#endif
#include "global.h"

#include "std_wrapper.h"

#if JE_DATA_STRUCTS_STD_BACKEND

#include "mem/mem_manager.h"

namespace je { namespace mem {

    void* std_wrapper_allocator_helper::allocate_internal(size a_num_bytes, size a_alignment)
    {
        return mem_manager::get_inst()
            .get_allocator_collections().allocate(a_num_bytes, static_cast<je::mem::alignment>(a_alignment));
    }

    void std_wrapper_allocator_helper::free_internal(void* a_memory)
    {
        mem_manager::get_inst()
            .get_allocator_collections().free(a_memory);
    }

}}

#endif
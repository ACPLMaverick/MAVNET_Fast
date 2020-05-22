#include "constant_allocator.h"

namespace je { namespace mem {

    constant_allocator::constant_allocator(
            base_allocator& a_allocator_from,
            size_t a_num_bytes,
            alignment a_alignment /*= k_default_alignment */)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment)
    {
    }

    constant_allocator::~constant_allocator()
    {
    }

    void* constant_allocator::allocate_internal(size_t a_num_bytes,
        alignment a_alignment)
    {

    }
    
    size_t constant_allocator::free_internal(void* a_memory)
    {
        JE_fail("Cannot free memory from constant allocator.");
        return 0;
    }

}}
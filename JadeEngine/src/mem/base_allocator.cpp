#include "base_allocator.h"

namespace JE_NAMESPACE { namespace mem { 

    base_allocator::base_allocator()
#if JE_TRACK_ALLOCATIONS
        : m_num_allocations(0)
#endif
    {

    }

    base_allocator::~base_allocator()
    {
#if JE_TRACK_ALLOCATIONS
        JE_assert(m_num_allocations == 0, "Memory leak.");
#endif
    }

    void* base_allocator::allocate(size_t a_num_bytes, size_t a_alignment)
    {
        void* mem = allocate_internal(a_num_bytes, a_alignment);
#if JE_TRACK_ALLOCATIONS
        m_num_allocations += static_cast<size_t>(mem != nullptr);
#endif
        JE_assert(mem != nullptr, "Allocation failed.");
        return mem;
    }

    void base_allocator::free(void* a_memory)
    {
        const bool free_success = free_internal(a_memory);
#if JE_TRACK_ALLOCATIONS
        m_num_allocations -= static_cast<size_t>(free_success);
#endif
        JE_assert(free_success, "Free failed.");
    }
}}
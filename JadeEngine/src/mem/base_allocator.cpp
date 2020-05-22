#include "base_allocator.h"

namespace je { namespace mem { 

    base_allocator::base_allocator()
#if JE_TRACK_ALLOCATIONS
        : m_num_allocations(0)
#endif
    {

    }

    base_allocator::~base_allocator()
    {
#if JE_TRACK_ALLOCATIONS
        JE_assert(m_num_allocations == 0 && m_used_memory == 0, "Memory leak.");
#endif
    }

    void* base_allocator::allocate(size_t a_num_bytes, size_t a_alignment)
    {
        void* mem = allocate_internal(a_num_bytes, a_alignment);
#if JE_TRACK_ALLOCATIONS
        if(mem != nullptr)
        {
            ++m_num_allocations;
            m_used_memory += a_num_bytes;
        }
#endif
        JE_assert(mem != nullptr, "Allocation failed.");
        return mem;
    }

    void base_allocator::free(void* a_memory)
    {
        const size_t freed = free_internal(a_memory);
#if JE_TRACK_ALLOCATIONS
        if(freed != 0)
        {
            --m_num_allocations;
            m_used_memory -= freed;
        }
#endif
        JE_assert(freed != 0, "Free failed.");
    }
}}
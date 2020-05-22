#include "base_allocator.h"

namespace je { namespace mem { 

    base_allocator::base_allocator()
        : m_allocator_from(nullptr)
        , m_memory(nullptr)
        , m_memory_num_bytes(0)
#if JE_TRACK_ALLOCATIONS
        , m_num_allocations(0)
        , m_used_num_bytes(0)
#endif
    {

    }

    base_allocator::base_allocator
    (
        base_allocator& a_allocator_from,
        size_t a_num_bytes,
        alignment a_alignment /*= k_default_alignment */
    )
        : m_allocator_from(&a_allocator_from)
        , m_memory(a_allocator_from.allocate(a_num_bytes, a_alignment))
        , m_memory_num_bytes(a_num_bytes)
#if JE_TRACK_ALLOCATIONS
        , m_num_allocations(0)
        , m_used_num_bytes(0)
#endif
    {

    }

    base_allocator::~base_allocator()
    {
        if(m_allocator_from != nullptr)
        {
            m_allocator_from->free(m_memory);
            m_memory = nullptr;
            m_memory_num_bytes = 0;
            m_allocator_from = nullptr;
        }
#if JE_TRACK_ALLOCATIONS
        JE_assert(m_num_allocations == 0 && m_used_num_bytes == 0, "Memory leak.");
#endif
    }

    void* base_allocator::allocate(size_t a_num_bytes, alignment a_alignment)
    {
        JE_assert(a_num_bytes <= m_memory_num_bytes,
            "Allocation too big for an allocator.");
        JE_assert(a_num_bytes % alignment_to_num(a_alignment) == 0,
            "Number of bytes is not a multiple of alignment.");
#if JE_TRACK_ALLOCATIONS
        JE_assert(a_num_bytes <= (m_memory_num_bytes - m_used_num_bytes),
            "Not enough memory in allocator.");
#endif

        void* mem = allocate_internal(a_num_bytes, a_alignment);

#if JE_TRACK_ALLOCATIONS
        if(mem != nullptr)
        {
            ++m_num_allocations;
            m_used_num_bytes += a_num_bytes;
        }
#endif

        JE_assert(mem != nullptr, "Allocation failed.");
        return mem;
    }

    void base_allocator::free(void* a_memory)
    {
#if JE_TRACK_ALLOCATIONS
        if(m_memory != nullptr)
        {
            const uintptr_t memory_uint = reinterpret_cast<uintptr_t>(a_memory);
            JE_assert(memory_uint >= get_memory_uint()
                && memory_uint < get_memory_uint() + m_memory_num_bytes,
                "Freed memory does not belong to this allocator.");
        }
#endif

        const size_t freed = free_internal(a_memory);
#if JE_TRACK_ALLOCATIONS
        if(freed != 0)
        {
            --m_num_allocations;
            m_used_num_bytes -= freed;
        }
#endif
        JE_assert(freed != 0, "Free failed.");
    }

    void* base_allocator::align_memory(void* a_memory, alignment a_alignment)
    {
        if(is_memory_aligned(a_memory, a_alignment))
        {
            return a_memory;
        }
        else
        {
            const uintptr_t num = memory_to_uint(a_memory);
            const uintptr_t alignment_num = alignment_to_uint(a_alignment);

            return uint_to_memory((num + alignment_num) & (~alignment_num));
        }
    }

    bool base_allocator::is_memory_aligned(void* a_memory, alignment a_alignment)
    {
        if(a_alignment == alignment::k_0)
        {
            return true;
        }

        const uintptr_t num = memory_to_uint(a_memory);
        const uintptr_t alignment_num = alignment_to_uint(a_alignment);

        const uintptr_t all_ones = alignment_num - 1;
        return (num & all_ones) == 0;
    }
}}
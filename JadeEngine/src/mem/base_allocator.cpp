#include "base_allocator.h"

namespace je { namespace mem { 

    base_allocator::base_allocator()
        : m_allocator_from(nullptr)
        , m_memory(nullptr)
        , m_memory_num_bytes(0)
#if JE_DEBUG_ALLOCATIONS
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
#if JE_DEBUG_ALLOCATIONS
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
            m_allocator_from = nullptr;
        }
#if JE_DEBUG_ALLOCATIONS
        JE_assert(m_num_allocations == 0 && m_used_num_bytes == 0, "Memory leak.");
#endif
    }

    void* base_allocator::allocate(size_t a_num_bytes, alignment a_alignment)
    {
        JE_assert_bailout(a_num_bytes > 0, nullptr,
            "Zero allocation is not possible.");
        JE_assert_bailout(a_num_bytes <= m_memory_num_bytes, nullptr,
            "Allocation too big for an allocator. Alloc: [%lld], Space: [%lld]", a_num_bytes, m_memory_num_bytes);
#if JE_DEBUG_ALLOCATIONS
        JE_assert(a_num_bytes <= (m_memory_num_bytes - m_used_num_bytes),
            "Not enough memory in allocator. Has: [%lld], Needs: [%lld], Lacks: [%lld]", m_memory_num_bytes - m_used_num_bytes,
                a_num_bytes, a_num_bytes - (m_memory_num_bytes - m_used_num_bytes));
#endif

        size_t bytes_allocated = 0;
        void* mem = allocate_internal(a_num_bytes, a_alignment, bytes_allocated);

#if JE_DEBUG_ALLOCATIONS
        if(mem != nullptr)
        {
            ++m_num_allocations;
            m_used_num_bytes += bytes_allocated;
        }
#endif

#if JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC
        if(mem != nullptr)
        {
            memset(mem, 0xCD, a_num_bytes);
        }
#endif

        JE_assert(mem != nullptr && bytes_allocated != 0, "Allocate failed.");
        JE_assert(mem_ptr(mem).is_aligned(a_alignment), "Pointer is not properly aligned. Is: [%p], Needs: [%p]",
            mem, mem_ptr(mem, a_alignment).get());

        return mem;
    }

    void base_allocator::free(void* a_memory)
    {
#if JE_DEBUG_ALLOCATIONS
        JE_assert_bailout(m_used_num_bytes > 0, , "Trying to free from an empty allocator.");
        if(m_memory != nullptr)
        {
            const uintptr_t memory_uint = reinterpret_cast<uintptr_t>(a_memory);
            const mem_ptr m_memory_ptr(m_memory);
            JE_assert(memory_uint >= m_memory_ptr
                && memory_uint < m_memory_ptr + m_memory_num_bytes,
                "Freed memory does not belong to this allocator.");
        }
#endif

        size_t num_bytes_freed = 0;
        const bool free_succeeded = free_internal(a_memory, num_bytes_freed);

#if JE_DEBUG_ALLOCATIONS
        if(free_succeeded)
        {
            --m_num_allocations;
            m_used_num_bytes -= num_bytes_freed;
        }
#endif

        JE_assert(free_succeeded, "Free failed.");
    }

    alignment base_allocator::get_alignment(mem_ptr a_memory)
    {
        // TODO Use enum util for this.

        if(a_memory.is_aligned(alignment::k_16))
        {
            return alignment::k_16;
        }
        else if(a_memory.is_aligned(alignment::k_64))
        {
            return alignment::k_64;
        }
        else if(a_memory.is_aligned(alignment::k_32))
        {
            return alignment::k_32;
        }
        else if(a_memory.is_aligned(alignment::k_8))
        {
            return alignment::k_8;
        }
        else if(a_memory.is_aligned(alignment::k_4))
        {
            return alignment::k_4;
        }
        else
        {
            JE_fail("Pointer is not aligned!");
            return alignment::k_0;
        }
    }

    void base_allocator::mem_ptr::align(alignment a_alignment)
    {
        if(a_alignment == alignment::k_0)
        {
            return;
        }
        else
        {
            const uintptr_t num = *this;
            const uintptr_t alignment_num = static_cast<uintptr_t>(a_alignment);
            const uintptr_t all_ones = alignment_num - 1;

            *this = ((num + alignment_num) & (~all_ones));
        }
    }

    size_t base_allocator::mem_ptr::align_adjust(alignment a_alignment, size_t a_additional_num_bytes)
    {
        size_t old = *this;
        *this += a_additional_num_bytes;
        align(a_alignment);
        return *this - old;
    }

    bool base_allocator::mem_ptr::is_aligned(alignment a_alignment)
    {
        if(a_alignment == alignment::k_0)
        {
            return true;
        }

        const uintptr_t num = *this;
        const uintptr_t alignment_num = static_cast<uintptr_t>(a_alignment);

        const uintptr_t all_ones = alignment_num - 1;
        return (num & all_ones) == 0;
    }
}}
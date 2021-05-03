#include "base_allocator.h"

#include <algorithm>

namespace je { namespace mem { 

    base_allocator::base_allocator(const char* a_name/* = nullptr*/, allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/)
        : m_allocator_from(nullptr)
        , m_memory(nullptr)
        , m_memory_num_bytes(0)
#if JE_DEBUG_ALLOCATIONS
        , m_child_allocators()
        , m_num_allocations(0)
        , m_used_num_bytes(0)
        , m_name(a_name)
        , m_debug_flags(a_debug_flags)
#endif
    {

    }

    base_allocator::base_allocator
    (
        base_allocator& a_allocator_from,
        size a_num_bytes,
        alignment a_alignment /*= k_default_alignment */,
        const char* a_name/* = nullptr*/,
        allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/
    )
        : m_allocator_from(&a_allocator_from)
        , m_memory(a_allocator_from.allocate(a_num_bytes, a_alignment))
        , m_memory_num_bytes(a_num_bytes)
#if JE_DEBUG_ALLOCATIONS
        , m_child_allocators()
        , m_num_allocations(0)
        , m_used_num_bytes(0)
        , m_name(a_name)
        , m_debug_flags(a_debug_flags)
#endif
    {
#if JE_DEBUG_ALLOCATIONS
        m_allocator_from->m_child_allocators.push_back(this);
#endif
    }

    base_allocator::~base_allocator()
    {
        if(m_allocator_from != nullptr)
        {
            m_allocator_from->free(m_memory);
#if JE_DEBUG_ALLOCATIONS
            JE_assert(m_child_allocators.size() == 0, "Destroying allocator which has child allocators!");

            auto it = std::find(m_allocator_from->m_child_allocators.begin(),
                m_allocator_from->m_child_allocators.end(), this);
            JE_assert(it != m_allocator_from->m_child_allocators.end());
            m_allocator_from->m_child_allocators.erase(it);
#endif
            m_memory = nullptr;
            m_allocator_from = nullptr;
        }

#if JE_DEBUG_ALLOCATIONS
        JE_assert(m_num_allocations == 0 && m_used_num_bytes == 0, "Memory leak.");
#endif
    }

    void* base_allocator::allocate(size a_num_bytes, alignment a_alignment)
    {
        if(debug_process_before_allocate(a_num_bytes, a_alignment) == false)
        {
            return nullptr;
        }

        size bytes_allocated = 0;
        void* mem = allocate_internal(a_num_bytes, a_alignment, bytes_allocated);

        debug_process_after_allocate(a_num_bytes, a_alignment, mem, bytes_allocated);

        return mem;
    }

    void base_allocator::free(void* a_memory)
    {
        if(debug_process_before_free(a_memory) == false)
        {
            return;
        }

        size num_bytes_freed = 0;
        const bool free_succeeded = free_internal(a_memory, num_bytes_freed);

        debug_process_after_free(free_succeeded, num_bytes_freed);
    }

    bool base_allocator::debug_process_before_allocate(size a_num_bytes, alignment a_alignment)
    {
        JE_assert_bailout(a_num_bytes > 0, false,
            "Zero allocation is not possible.");
        JE_assert_bailout(a_num_bytes <= m_memory_num_bytes, false,
            "Allocation too big for an allocator. Alloc: [%lld], Space: [%lld]", a_num_bytes, m_memory_num_bytes);
#if JE_DEBUG_ALLOCATIONS
        JE_assert(is_alignment_correct(a_alignment), "Non-supported alignment.");
        if(m_debug_flags & allocator_debug_flags::k_count_allocations)
        {
            JE_assert(a_num_bytes <= (m_memory_num_bytes - m_used_num_bytes),
                "Not enough memory in allocator. Has: [%lld], Needs: [%lld], Lacks: [%lld]", m_memory_num_bytes - m_used_num_bytes,
                    a_num_bytes, a_num_bytes - (m_memory_num_bytes - m_used_num_bytes));
        }
#endif
        return true;
    }

    void base_allocator::debug_process_after_allocate(size a_num_bytes, alignment a_alignment, void* a_memory, size a_bytes_allocated)
    {
#if JE_DEBUG_ALLOCATIONS
        if(m_debug_flags & allocator_debug_flags::k_count_allocations)
        {
            if(a_memory != nullptr)
            {
                ++m_num_allocations;
                m_used_num_bytes += a_bytes_allocated;
            }
        }
#endif

#if JE_DEBUG_ALLOCATIONS_FILL_MEMORY_ON_ALLOC
        if(m_debug_flags & allocator_debug_flags::k_fill_memory_on_alloc)
        {
            if(a_memory != nullptr)
            {
                memset(a_memory, 0xCD, a_num_bytes);
            }
        }
#endif

#if JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER
        if(m_debug_flags & allocator_debug_flags::k_stack_tracer)
        {
            if(a_memory != nullptr)
            {
                m_stack_tracer.capture_trace(a_memory);
            }
        }
#endif

        JE_assert(a_memory != nullptr && a_bytes_allocated != 0, "Allocate failed.");
        JE_assert(mem_ptr(a_memory).is_aligned(a_alignment), "Pointer is not properly aligned. Is: [%p], Needs: [%p]",
            a_memory, mem_ptr(a_memory, a_alignment).get());
    }

    bool base_allocator::debug_process_before_free(void* a_memory)
    {
#if JE_DEBUG_ALLOCATIONS
        if(m_debug_flags & allocator_debug_flags::k_count_allocations)
        {
            JE_assert_bailout(m_used_num_bytes > 0, false, "Trying to free from an empty allocator.");
        }

        if(m_memory != nullptr)
        {
            const uptr memory_uint = reinterpret_cast<uptr>(a_memory);
            const mem_ptr m_memory_ptr(m_memory);
            JE_assert(memory_uint >= m_memory_ptr
                && memory_uint < m_memory_ptr + m_memory_num_bytes,
                "Freed memory does not belong to this allocator.");
        }
#endif

#if JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER
        if(m_debug_flags & allocator_debug_flags::k_stack_tracer)
        {
            if(a_memory != nullptr)
            {
                m_stack_tracer.remove_trace(a_memory);
            }
        }
#endif
        return true;
    }

    void base_allocator::debug_process_after_free(bool a_free_succeeded, size a_num_bytes_freed)
    {
#if JE_DEBUG_ALLOCATIONS
        if(m_debug_flags & allocator_debug_flags::k_count_allocations)
        {
            if(a_free_succeeded)
            {
                --m_num_allocations;
                m_used_num_bytes -= a_num_bytes_freed;
            }
        }
#endif

        JE_assert(a_free_succeeded, "Free failed.");
    }

    void base_allocator::conditionally_print_stack_trace()
    {
#if JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER
        if(m_debug_flags & allocator_debug_flags::k_stack_tracer)
        {
            if(m_stack_tracer.get_num_remaining_traces() > 0)
            {
                m_stack_tracer.print_remaining_traces();
            }
        }
#endif
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
            return alignment::k_1;
        }
    }

    bool base_allocator::is_alignment_correct(alignment a_alignment)
    {
        switch (a_alignment)
        {
        case alignment::k_1:
        case alignment::k_4:
        case alignment::k_8:
        case alignment::k_16:
        case alignment::k_32:
        case alignment::k_64:
            return true;
        default:
            return false;
        }
    }

    void base_allocator::mem_ptr::align(alignment a_alignment)
    {
        if(a_alignment == alignment::k_1)
        {
            return;
        }
        else
        {
            const uptr num = *this;
            const uptr alignment_num = static_cast<uptr>(a_alignment);
            const uptr all_ones = alignment_num - 1;

            *this = ((num + alignment_num) & (~all_ones));
        }
    }

    size base_allocator::mem_ptr::align_adjust(alignment a_alignment, size a_additional_num_bytes)
    {
        size old = *this;
        *this += a_additional_num_bytes;
        align(a_alignment);
        return *this - old;
    }

    bool base_allocator::mem_ptr::is_aligned(alignment a_alignment)
    {
        if(a_alignment == alignment::k_1)
        {
            return true;
        }

        const uptr num = *this;
        const uptr alignment_num = static_cast<uptr>(a_alignment);

        const uptr all_ones = alignment_num - 1;
        return (num & all_ones) == 0;
    }
}}
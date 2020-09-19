#include "linear_allocator.h"

namespace je { namespace mem {

    linear_allocator::linear_allocator(
            base_allocator& a_allocator_from,
            size_t a_num_bytes,
            alignment a_alignment /*= k_default_alignment */,
            const char* a_name/* = nullptr*/,
            allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment, a_name, a_debug_flags)
        , m_memory_head(m_memory)
    {
    }

    linear_allocator::~linear_allocator()
    {
        clear();
    }

    void linear_allocator::clear()
    {
#if JE_DEBUG_ALLOCATIONS
        if(m_debug_flags & allocator_debug_flags::k_count_allocations)
        {
            m_used_num_bytes = 0;
            m_num_allocations = 0;
        }
#endif
#if JE_DEBUG_ALLOCATIONS_USE_STACK_TRACER
        if(m_debug_flags & allocator_debug_flags::k_stack_tracer)
        {
            m_stack_tracer.clear();
        }
#endif

        m_memory_head = m_memory;
    }

    linear_allocator::mem_ptr linear_allocator::allocate_internal(size_t a_num_bytes,
        alignment a_alignment, size_t& a_out_num_bytes_allocated)
    {
        mem_ptr aligned_head(m_memory_head, a_alignment);
        const size_t num_bytes_left = m_memory_num_bytes - (aligned_head - mem_ptr(m_memory));

#if JE_DEBUG_ALLOCATIONS
        if(num_bytes_left < a_num_bytes)
        {
            JE_fail("Not enough memory for allocation. "
                "Has: [%lld], Needs: [%lld], Lacks: [%lld]",
                num_bytes_left, a_num_bytes, a_num_bytes - num_bytes_left);
            return nullptr;
        }
#endif

        a_out_num_bytes_allocated = aligned_head - mem_ptr(m_memory_head) + a_num_bytes;

        m_memory_head = mem_ptr(aligned_head + a_num_bytes);
        return aligned_head;
    }
    
    bool linear_allocator::free_internal(mem_ptr a_memory, size_t& a_out_num_bytes_freed)
    {
        // Does nothing and does not internally free any memory.
        a_out_num_bytes_freed = 0;
        return true;
    }

}}
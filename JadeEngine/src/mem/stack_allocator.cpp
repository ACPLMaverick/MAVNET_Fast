#include "stack_allocator.h"

namespace je { namespace mem { 

    stack_allocator::stack_allocator(
            base_allocator& a_allocator_from,
            size_t a_num_bytes,
            alignment a_alignment /*= k_default_alignment*/)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment)
        , m_memory_head(m_memory)
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        , m_prev_head_aligned(m_memory_head)
#endif
    {
    }

    stack_allocator::~stack_allocator()
    {
        conditionally_print_stack_trace();
        JE_assert(m_memory_head == m_memory, "Memory leak.");
    }

    stack_mem stack_allocator::allocate_stack_mem(size_t a_num_bytes,
        alignment a_alignment /*= base_allocator::k_default_alignment*/)
    {
        return stack_mem(*this, a_num_bytes, a_alignment);
    }

    stack_allocator::mem_ptr stack_allocator::allocate_internal(size_t a_num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated)
    {
        const mem_ptr head(m_memory_head);
        mem_ptr aligned_head(m_memory_head);
        const size_t align_adjustment = 
            aligned_head.align_adjust(a_alignment, sizeof(control_block)); 
            // This assures that I have at least sizeof(control_block) free space before.

        mem_ptr next_head(aligned_head + a_num_bytes);

#if JE_DEBUG_ALLOCATIONS
        if(next_head > mem_ptr(mem_ptr(m_memory) + m_memory_num_bytes))
        {
            const size_t num_bytes_left = mem_ptr(m_memory) + m_memory_num_bytes - aligned_head;
            JE_fail("Not enough memory for allocation. "
                "Has: [%lld], Needs: [%lld], Lacks: [%lld]",
                num_bytes_left, a_num_bytes, a_num_bytes - num_bytes_left);
            return nullptr;
        }
#endif

        control_block* cb = aligned_head.get_struct_ptr_before<control_block>();
        out_num_bytes_allocated = next_head - head;
        cb->m_alignment_num_bytes = align_adjustment;

#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        cb->m_prev_block_num_bytes = aligned_head - mem_ptr(m_prev_head_aligned);
        m_prev_head_aligned = aligned_head;
#endif

        m_memory_head = next_head;

        return aligned_head;
    }

    bool stack_allocator::free_internal(mem_ptr a_memory, size_t& a_out_num_bytes_freed)
    {
#if JE_DEBUG_ALLOCATIONS
        if(m_memory_head == m_memory)
        {
            JE_fail("Trying to free from empty allocator.");
            return false;
        }
#endif
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        if(a_memory.get() != m_prev_head_aligned)
        {
            JE_fail("Trying to free non-top block.");
            return false;
        }
#endif

        control_block* cb = a_memory.get_struct_ptr_before<control_block>();
        JE_assert(cb->m_alignment_num_bytes <= static_cast<uint8_t>(alignment::k_64), "Sanity check for control block failed.");
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        JE_assert(cb->m_prev_block_num_bytes != 0 && cb->m_prev_block_num_bytes < (8ULL * k_GB),
            "Sanity check for control block failed.");
#endif

        mem_ptr prev_head_nonaligned = mem_ptr(a_memory - cb->m_alignment_num_bytes);
        a_out_num_bytes_freed = mem_ptr(m_memory_head) - prev_head_nonaligned;
        m_memory_head = prev_head_nonaligned;

#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        m_prev_head_aligned = mem_ptr(a_memory - cb->m_prev_block_num_bytes);

        if(m_memory_head != m_memory)
        {
            // Check if this pointer is really aligned.
            JE_assert(get_alignment(m_prev_head_aligned) != alignment::k_0, "Bad prev pointer alignment");
            // Check previous control block.
            control_block* pcb = reinterpret_cast<control_block*>(mem_ptr(m_prev_head_aligned) - sizeof(control_block));
        JE_assert(pcb->m_alignment_num_bytes <= static_cast<uint8_t>(alignment::k_64), "Sanity check for control block failed.");
        JE_assert(pcb->m_prev_block_num_bytes != 0 && pcb->m_prev_block_num_bytes < (8ULL * k_GB),
            "Sanity check for control block failed.");
        }
#endif

        return true;
    }

    stack_mem::stack_mem(stack_allocator& allocator, size_t a_num_bytes,
        alignment a_alignment /*= base_allocator::k_default_alignment*/)
        : m_allocator(allocator)
        , m_memory(allocator.allocate(a_num_bytes, a_alignment))
    {
    }

    stack_mem::~stack_mem()
    {
        m_allocator.free(m_memory);
    }

}}
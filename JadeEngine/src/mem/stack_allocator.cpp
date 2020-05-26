#include "stack_allocator.h"

namespace je { namespace mem { 

    stack_allocator::stack_allocator(
            base_allocator& a_allocator_from,
            size_t a_num_bytes,
            alignment a_alignment /*= k_default_alignment*/)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment)
        , m_memory_head(m_memory)
    {
    }

    stack_allocator::~stack_allocator()
    {
    }

    stack_mem stack_allocator::allocate_stack_mem(size_t a_num_bytes,
        alignment a_alignment /*= base_allocator::k_default_alignment*/)
    {
        return stack_mem(*this, a_num_bytes, a_alignment);
    }

    stack_allocator::mem_ptr stack_allocator::allocate_internal(size_t a_num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated)
    {
        const mem_ptr head(m_memory_head);
        const mem_ptr aligned_head(m_memory_head, a_alignment);
        mem_ptr next_head(aligned_head + a_num_bytes);

#if JE_DEBUG_ALLOCATIONS
        if(next_head > mem_ptr(m_memory) + m_memory_num_bytes)
        {
            const size_t num_bytes_left = mem_ptr(m_memory) + m_memory_num_bytes - aligned_head;
            JE_fail("Not enough memory for allocation. "
                "Has: [%lld], Needs: [%lld], Lacks: [%lld]",
                num_bytes_left, a_num_bytes, a_num_bytes - num_bytes_left);
            return nullptr;
        }
#endif

        control_block* cb = reinterpret_cast<control_block*>(next_head.get());
        out_num_bytes_allocated = next_head - head;
        cb->m_prev_block_num_bytes = out_num_bytes_allocated;
        next_head += sizeof(control_block);
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

        const mem_ptr head(m_memory_head);
        control_block* cb = reinterpret_cast<control_block*>(head - sizeof(control_block));
        JE_assert(cb->m_prev_block_num_bytes != 0 && cb->m_prev_block_num_bytes < (8ULL * k_GB),
            "Sanity check for control block failed.");

        mem_ptr last_head = head - cb->m_prev_block_num_bytes - sizeof(control_block);
        
#if JE_DEBUG_ALLOCATIONS
        if(last_head != a_memory)
        {
            // This looks way too complex but I'm leaving it for now.

            alignment mem_alignment = get_alignment(a_memory);
            mem_ptr last_head_aligned(last_head);
            last_head_aligned.align(mem_alignment);
            
            if(last_head_aligned != a_memory)
            {
                JE_fail("Trying to free non-top block.");
                return false;
            }
        }
#endif

        a_out_num_bytes_freed = cb->m_prev_block_num_bytes;
        m_memory_head = last_head.get();

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
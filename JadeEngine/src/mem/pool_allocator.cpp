 #include "pool_allocator.h"

 #include <algorithm>

 namespace je { namespace mem { 

    pool_allocator::pool_allocator(
        base_allocator& a_allocator_from,
        size a_object_num_bytes,
        size a_num_objects,
        alignment a_alignment /*= k_default_alignment*/,
        const char* a_name/* = nullptr*/,
        allocator_debug_flags a_debug_flags /*= base_allocator::k_default_debug_flags*/)
        : base_allocator(a_allocator_from, 
            std::max(a_object_num_bytes, sizeof(free_block))  * a_num_objects, a_alignment, a_name, a_debug_flags)
        , m_first_free_block(mem_ptr(m_memory).cast<free_block>())
        , m_object_num_bytes(std::max(a_object_num_bytes, sizeof(free_block)))
#if JE_DEBUG_ALLOCATIONS
        , m_alignment(a_alignment)
#endif
    {
        m_first_free_block->m_next = nullptr;
    }

    pool_allocator::~pool_allocator()
    {
        conditionally_print_stack_trace();
    }

    void* pool_allocator::allocate()
    {
        const alignment al =
#if JE_DEBUG_ALLOCATIONS
        m_alignment;
#else
        k_default_alignment;
#endif

        return base_allocator::allocate(m_object_num_bytes, al);
    }

    pool_allocator::mem_ptr pool_allocator::allocate_internal(
        size a_num_bytes, alignment a_alignment, size& a_out_num_bytes_allocated)
    {
#if JE_DEBUG_ALLOCATIONS
        JE_assert(a_num_bytes == m_object_num_bytes, "Passing wrong num_bytes to allocate function of a pool!")
        JE_assert(a_alignment == get_alignment(m_memory), "Passing wrong alignment to allocate function of a pool!");
        JE_assert_bailout(mem_ptr(m_first_free_block) <= mem_ptr(mem_ptr(m_memory) + m_memory_num_bytes),
            nullptr, "Not enough memory in allocator.");
#endif

        mem_ptr memory(m_first_free_block);

        if(m_first_free_block->m_next == nullptr)
        {
            // If we don't have any *marked* free blocks after this one,
            // just append first free block.
            free_block* temp = m_first_free_block;
            m_first_free_block = 
                mem_ptr(mem_ptr(m_first_free_block) + m_object_num_bytes).cast<free_block>();
            
            m_first_free_block->m_next = temp->m_next;
        }
        else
        {
            // We have a *marked* free block after this one. Just assign it. 
            m_first_free_block = m_first_free_block->m_next;
        }

        a_out_num_bytes_allocated = m_object_num_bytes;

        return memory;
    }

    bool pool_allocator::free_internal(
        mem_ptr a_memory, size& a_out_num_bytes_freed)
    {
        free_block* this_free_block = a_memory.cast<free_block>();
        this_free_block->m_next = m_first_free_block;
        m_first_free_block = this_free_block;

        a_out_num_bytes_freed = m_object_num_bytes;
        return true;
    }

 }}
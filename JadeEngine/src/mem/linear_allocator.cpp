#include "linear_allocator.h"

namespace je { namespace mem {

    linear_allocator::linear_allocator(
            base_allocator& a_allocator_from,
            size_t a_num_bytes,
            alignment a_alignment /*= k_default_alignment */)
        : base_allocator(a_allocator_from, a_num_bytes, a_alignment)
        , m_memory_head(m_memory)
    {
    }

    linear_allocator::~linear_allocator()
    {
    }

    void linear_allocator::clear()
    {
#if JE_DEBUG_ALLOCATIONS
        m_used_num_bytes = 0;
        m_num_allocations = 0;
#endif

        m_memory_head = m_memory;
    }

    void* linear_allocator::allocate_internal(size_t a_num_bytes,
        alignment a_alignment)
    {
        void* aligned_head = align_memory(m_memory_head, a_alignment);
        const size_t num_bytes_left = m_memory_num_bytes - (memory_to_uint(aligned_head) - memory_to_uint(m_memory));
        if(num_bytes_left < a_num_bytes)
        {
            return nullptr;
        }

        m_memory_head = uint_to_memory(memory_to_uint(aligned_head) + a_num_bytes);
        return aligned_head;
    }
    
    bool linear_allocator::free_internal(void* a_memory, size_t& a_out_num_bytes_freed)
    {
        // Does nothing and does not internally free any memory.
        return true;
    }

}}
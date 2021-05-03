#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class pool_allocator : public base_allocator
    {
    public:

        pool_allocator(
            base_allocator& allocator_from,
            size object_num_bytes,
            size num_objects,
            alignment a_alignment = k_default_alignment,
            const char* name = nullptr,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~pool_allocator();

        JE_disallow_copy(pool_allocator);

        void* allocate();
        size get_object_num_bytes() const { return m_object_num_bytes; }

    protected:

        struct free_block
        {
            free_block* m_next;
        };

        virtual mem_ptr allocate_internal(size num_bytes, alignment a_alignment, size& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size& out_num_bytes_freed) override final;

        size get_num_objects() const { return m_memory_num_bytes / m_object_num_bytes; }

        free_block* m_first_free_block;
        size m_object_num_bytes;
#if JE_DEBUG_ALLOCATIONS
        alignment m_alignment;
#endif
    };

}}
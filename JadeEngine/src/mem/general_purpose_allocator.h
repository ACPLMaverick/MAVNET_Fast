#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class general_purpose_allocator : public base_allocator
    {
    public:

        general_purpose_allocator(
            base_allocator& allocator_from,
            size num_bytes,
            alignment a_alignment = k_default_alignment,
            const char* name = nullptr,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~general_purpose_allocator();

        JE_disallow_copy(general_purpose_allocator);

    protected:

        struct allocation_header
        {
            size m_num_bytes;
            u8 m_adjustment_num_bytes;
        };

        struct free_block
        {
            size m_num_bytes;
            free_block* m_next;
        };

        virtual mem_ptr allocate_internal(size num_bytes, alignment a_alignment, size& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size& out_num_bytes_freed) override final;


        free_block* m_free_blocks;
    };

}}
#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class linear_allocator : public base_allocator
    {
    public:

        linear_allocator(
            base_allocator& allocator_from,
            size_t num_bytes,
            alignment a_alignment = k_default_alignment,
            const char* name = nullptr,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~linear_allocator();

        JE_disallow_copy(linear_allocator);

        void clear();

    protected:

        virtual mem_ptr allocate_internal(size_t num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size_t& out_num_bytes_freed) override final;

        void* m_memory_head;
    };

}}
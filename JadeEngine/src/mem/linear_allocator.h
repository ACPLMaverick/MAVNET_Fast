#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class linear_allocator : public base_allocator
    {
    public:

        linear_allocator(
            base_allocator& allocator_from,
            size_t num_bytes,
            alignment a_alignment = k_default_alignment);
        ~linear_allocator();

        void clear();

    protected:

        virtual void* allocate_internal(size_t num_bytes, alignment a_alignment) override final;
        virtual bool free_internal(void* memory, size_t& out_num_bytes_freed) override final;

        void* m_memory_head;
    };

}}
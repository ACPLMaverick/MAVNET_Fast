#pragma once

#include "base_allocator.h"

namespace je { namespace mem { 

    class stack_allocator;

    class stack_mem
    {
    public:

        const void* get() const { return m_memory; }
        void* get() { return m_memory; }

        operator const void*() const { return get(); }
        operator void*() { return get(); }

    private:

        stack_mem(stack_allocator& allocator, size_t num_bytes,
            alignment a_alignment = base_allocator::k_default_alignment);

    public:
        ~stack_mem();

    private:

        stack_allocator& m_allocator;
        void* m_memory;

        friend class stack_allocator;
    };

    class stack_allocator : public base_allocator
    {
    public:

        stack_allocator(
            base_allocator& allocator_from,
            size_t num_bytes,
            alignment a_alignment = k_default_alignment);
        virtual ~stack_allocator();

        stack_mem allocate_stack_mem(size_t num_bytes,
            alignment a_alignment = base_allocator::k_default_alignment);

    protected:

        struct control_block
        {
            size_t m_prev_block_num_bytes;
        };

        virtual mem_ptr allocate_internal(size_t num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size_t& out_num_bytes_freed) override final;

        void* m_memory_head;
    };

}}
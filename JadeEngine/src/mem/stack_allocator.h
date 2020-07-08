#pragma once

#include "base_allocator.h"

#if JE_DEBUG_ALLOCATIONS
#define JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV 1
#else
#define JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV 0
#endif

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
            alignment a_alignment = k_default_alignment,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~stack_allocator();

        JE_disallow_copy(stack_allocator);

        stack_mem allocate_stack_mem(size_t num_bytes,
            alignment a_alignment = base_allocator::k_default_alignment);

    protected:

        struct control_block
        {
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
            size_t m_prev_block_num_bytes;
#endif
            uint8_t m_alignment_num_bytes;
        };

        virtual mem_ptr allocate_internal(size_t num_bytes, alignment a_alignment, size_t& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size_t& out_num_bytes_freed) override final;

        void* m_memory_head;
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        void* m_prev_head_aligned;
#endif
    };

}}
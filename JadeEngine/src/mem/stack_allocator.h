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

        stack_mem(stack_allocator& allocator, size num_bytes,
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
            size num_bytes,
            alignment a_alignment = k_default_alignment,
            const char* name = nullptr,
            allocator_debug_flags debug_flags = base_allocator::k_default_debug_flags);
        virtual ~stack_allocator();

        JE_disallow_copy(stack_allocator);

        stack_mem allocate_stack_mem(size num_bytes,
            alignment a_alignment = base_allocator::k_default_alignment);

    protected:

        struct control_block
        {
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
            size m_prev_block_num_bytes;
#endif
            u8 m_alignment_num_bytes;
        };

        virtual mem_ptr allocate_internal(size num_bytes, alignment a_alignment, size& out_num_bytes_allocated) override final;
        virtual bool free_internal(mem_ptr memory, size& out_num_bytes_freed) override final;

        void* m_memory_head;
#if JE_DEBUG_ALLOCATIONS_STACK_CHECK_PREV
        void* m_prev_head_aligned;
#endif
    };

}}
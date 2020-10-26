#pragma once

#include "global.h"

#include "system_allocator.h"
#include "linear_allocator.h"
#include "stack_allocator.h"
#include "general_purpose_allocator.h"
#include "pool_allocator.h"
#include "std_wrapper.h"

namespace je { namespace mem {

    // This is responsible for managing all heap memory in the application.
    // Stores all allocators and configurations regarding memory budgets.
    // Every object requests memory through this class via a template function.
    class mem_manager
    {
    public:

        mem_manager();
        ~mem_manager();

        template <typename caller_type>
        void* allocate(size_t a_num_bytes,
            alignment a_alignment = base_allocator::k_default_alignment)
        {
            return find_allocator<caller_type>().allocate(a_num_bytes, a_alignment);
        }

        template <typename caller_type>
        void free(void* a_memory)
        {
            find_allocator<caller_type>().free(a_memory);
        }

#if JE_DEBUG_ALLOCATIONS
        void print_memory_summary() const;
#endif

    private:

        struct mem_budgets
        {
            size_t m_total_byte_num;
            size_t m_one_frame_byte_num;
            size_t m_variable_byte_num;
            size_t m_collections_byte_num;
            size_t m_general_purpose_byte_num;
            size_t m_object_pool_byte_num;
            size_t m_resource_chunks_byte_num;
            size_t m_component_chunks_byte_num;

            mem_budgets();
            ~mem_budgets();

        private:

            static size_t get_percentage_of(size_t src_byte_num, size_t percentage);
        };

        template <typename caller_type>
        base_allocator& find_allocator()
        {
            // This is a default implementation for any caller type that has no allocator specified.
            // Return OS allocator.
            return m_system_allocator;
        }

#if JE_DATA_STRUCTS_STD_BACKEND
        template<>
        base_allocator& find_allocator<std_wrapper_allocator_helper>()
        {
            return m_collections_allocator;
        }
#endif

        mem_budgets m_mem_budgets;
        
        // Stored allocators.
        system_allocator m_system_allocator;
        linear_allocator m_top_allocator;

        linear_allocator m_one_frame_allocator;

        linear_allocator m_variable_allocator;
        general_purpose_allocator m_collections_allocator;
        general_purpose_allocator m_general_purpose_allocator;

        linear_allocator m_object_pool_allocator;
        // TODO object pool

        //stack_allocator m_stack_allocator;    // TODO stack allocator when necessary.
    };

}}
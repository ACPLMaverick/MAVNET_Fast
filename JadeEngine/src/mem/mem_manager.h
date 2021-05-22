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

        static mem_manager& get_inst();

        base_allocator& get_allocator_system() { return m_system_allocator; }
        base_allocator& get_allocator_one_frame() { return m_one_frame_allocator; }
        base_allocator& get_allocator_persistent() { return m_persistent_allocator; }
        base_allocator& get_allocator_general_purpose() { return m_general_purpose_allocator; }
        base_allocator& get_allocator_collections() { return m_collections_allocator; }

#if JE_DEBUG_ALLOCATIONS
        void print_memory_summary() const;
#endif

    private:

        struct mem_budgets
        {
            size m_total_byte_num;
            size m_one_frame_byte_num;
            size m_persistent_byte_num;
            size m_variable_byte_num;
            size m_collections_byte_num;
            size m_general_purpose_byte_num;
            size m_object_pool_byte_num;
            size m_resource_chunks_byte_num;
            size m_component_chunks_byte_num;

            mem_budgets();
            ~mem_budgets();

        private:

            static size get_percentage_of(size src_byte_num, size percentage);
        };

        mem_manager();
        ~mem_manager();

        mem_budgets m_mem_budgets;
        
        // Stored allocators.
        system_allocator m_system_allocator;
        linear_allocator m_top_allocator;

        linear_allocator m_one_frame_allocator;
        linear_allocator m_persistent_allocator;

        linear_allocator m_variable_allocator;
        general_purpose_allocator m_collections_allocator;
        general_purpose_allocator m_general_purpose_allocator;

        linear_allocator m_object_pool_allocator;
        // TODO object pool

        //stack_allocator m_stack_allocator;    // TODO stack allocator when necessary.
    };

}}
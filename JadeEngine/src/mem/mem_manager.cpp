#include "mem_manager.h"

namespace je { namespace mem { 

    mem_manager::mem_manager()
        : m_mem_budgets()
        , m_system_allocator()
        , m_top_allocator(m_system_allocator, m_mem_budgets.m_total_byte_num, alignment::k_16)
        , m_one_frame_allocator(m_top_allocator, m_mem_budgets.m_one_frame_byte_num, alignment::k_1)
        , m_variable_allocator(m_top_allocator, m_mem_budgets.m_variable_byte_num, alignment::k_1)
        , m_collections_allocator(m_variable_allocator, m_mem_budgets.m_collections_byte_num, alignment::k_1)
        , m_general_purpose_allocator(m_variable_allocator, m_mem_budgets.m_general_purpose_byte_num, alignment::k_1)
        , m_object_pool_allocator(m_top_allocator, m_mem_budgets.m_object_pool_byte_num, alignment::k_1)
    {

    }

    mem_manager::~mem_manager()
    {
    }

    mem_manager::mem_budgets::mem_budgets()
        : m_total_byte_num(64 * k_MB)
        , m_one_frame_byte_num(get_percentage_of(m_total_byte_num, 10))
        , m_variable_byte_num(get_percentage_of(m_total_byte_num, 40))
        , m_collections_byte_num(get_percentage_of(m_variable_byte_num, 50))
        , m_general_purpose_byte_num(m_variable_byte_num - m_collections_byte_num)
        , m_object_pool_byte_num(m_total_byte_num - m_one_frame_byte_num - m_variable_byte_num)
        , m_resource_chunks_byte_num(get_percentage_of(m_object_pool_byte_num, 50))
        , m_component_chunks_byte_num(m_object_pool_byte_num - m_resource_chunks_byte_num)
    {
        // TODO Load values from some file.
    }

    mem_manager::mem_budgets::~mem_budgets()
    {
    }

    size_t mem_manager::mem_budgets::get_percentage_of(size_t a_src_byte_num, size_t a_percentage)
    {
        const size_t perc = ((a_src_byte_num * a_percentage) / 100);
        return perc;
    }

}}
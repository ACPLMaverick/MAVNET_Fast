#include "mem_manager.h"

#if JE_DEBUG_ALLOCATIONS
#include <string>
#endif

namespace je { namespace mem { 

#if JE_DEBUG_ALLOCATIONS
    class byte_output
    {
    public:
        byte_output(size a_byte_num)
        {
            if(a_byte_num >= k_GB)
            {
                process_byte_num(k_GB, 'G', a_byte_num);
            }
            if(a_byte_num >= k_MB)
            {
                process_byte_num(k_MB, 'M', a_byte_num);
            }
            if(a_byte_num >= k_kB)
            {
                process_byte_num(k_kB, 'K', a_byte_num);
            }
            if(a_byte_num > 0)
            {
                m_text += std::to_string(a_byte_num) + " ";
            }
            else if(m_text.size() == 0)
            {
                m_text = "0 ";
            }

            // Remove trailing space.
            m_text.erase(m_text.end() - 1);
        }

        const std::string& as_string() const { return m_text; }
        
    private:

        void process_byte_num(size a_byte_unit, char a_unit_text, size& a_inout_byte_num)
        {
            const size byte_num_in_unit = a_inout_byte_num / a_byte_unit;
            m_text += std::to_string(byte_num_in_unit) + a_unit_text + " ";
            a_inout_byte_num -= (byte_num_in_unit * a_byte_unit);
        }

        std::string m_text;
    };

    class allocator_info_formatter
    {
    public:

        static std::string get_allocator_summary(const base_allocator& a_allocator, size a_hierarchy_level)
        {
            std::string summary;
            for(size i = 0; i < a_hierarchy_level; ++i)
            {
                summary.append("\t");
            }

            const byte_output output_capacity(a_allocator.get_total_memory());
            const byte_output output_used(a_allocator.get_used_memory());
            const byte_output output_free(a_allocator.get_memory_left());

            const char* format = "[%s] [%.2f%%] [capacity : %s] [used : %s] [free : %s] [allocs : %llu]\n";

            static const size buf_size = 256;
            static char buf[buf_size];

            const f32 perc = (static_cast<f32>(a_allocator.get_memory_left()) / static_cast<f32>(a_allocator.get_total_memory())) * 100.0f;

            const size size = snprintf(buf, buf_size, format, a_allocator.get_name(), perc,
                output_capacity.as_string().c_str(), output_used.as_string().c_str(), output_free.as_string().c_str(),
                a_allocator.get_num_allocations());
            JE_assert(size < buf_size);
            summary += std::string(buf);
            return summary;
        }
    };
#endif

    mem_manager::mem_manager()
        : m_mem_budgets()
        , m_system_allocator()
        , m_top_allocator(m_system_allocator, m_mem_budgets.m_total_byte_num, alignment::k_16, "Topmost")
        , m_one_frame_allocator(m_top_allocator, m_mem_budgets.m_one_frame_byte_num, alignment::k_1, "OneFrame")
        , m_variable_allocator(m_top_allocator, m_mem_budgets.m_variable_byte_num, alignment::k_1, "Variable")
        , m_collections_allocator(m_variable_allocator, m_mem_budgets.m_collections_byte_num, alignment::k_1, "Collection")
        , m_general_purpose_allocator(m_variable_allocator, m_mem_budgets.m_general_purpose_byte_num, alignment::k_1, "GenPurpose")
        , m_object_pool_allocator(m_top_allocator, m_mem_budgets.m_object_pool_byte_num, alignment::k_1, "ObjPool")
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

    size mem_manager::mem_budgets::get_percentage_of(size a_src_byte_num, size a_percentage)
    {
        const size perc = ((a_src_byte_num * a_percentage) / 100);
        return perc;
    }

#if JE_DEBUG_ALLOCATIONS
    void mem_manager::print_memory_summary() const
    {
        struct allocator_data
        {
            const base_allocator* allocator;
            size hierarchy_level;
        };

        const size total_memory_available = m_top_allocator.get_total_memory();
        size total_memory_used = 0;
        size total_num_allocations = 0;

        std::string summary;

        std::stack<allocator_data> allocators;
        allocators.push({&m_top_allocator, 0});
        while(allocators.size() > 0)
        {
            allocator_data data = allocators.top();
            allocators.pop();

            summary += allocator_info_formatter::get_allocator_summary(*data.allocator, data.hierarchy_level);

            // Collect memory usage info only from leaf allocators.
            if(data.allocator->get_child_allocators().size() == 0)
            {
                total_memory_used += data.allocator->get_used_memory();
            }
            total_num_allocations += data.allocator->get_num_allocations();

            for(const base_allocator* child_allocator : data.allocator->get_child_allocators())
            {
                allocators.push({child_allocator, data.hierarchy_level + 1});
            }
        }

        JE_print(
            "######################################"
            "\nTotal num allocations: [%zu]\n"
            "Total memory used: [%s]\n"
            "Total memory available [%s]\n%s"
            "######################################",
            total_num_allocations,
            byte_output(total_memory_used).as_string().c_str(),
            byte_output(total_memory_available).as_string().c_str(),
            summary.c_str());
    }
#endif
}}
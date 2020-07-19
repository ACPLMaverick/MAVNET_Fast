#include "stack_tracer.h"

#if JE_USE_STACK_TRACER

#include "platf/platf.h"

namespace je { namespace platf {

    stack_tracer::stack_tracer()
    {
    }

    stack_tracer::~stack_tracer()
    {
    }

    void stack_tracer::clear()
    {
        m_traces.clear();
    }

    void stack_tracer::capture_trace(key a_key)
    {
        stack_trace new_trace{};

        generate_trace(new_trace);

        if(new_trace.m_num_traces > 0)
        {
            m_traces.emplace(a_key, new_trace);
        }
        else
        {
            JE_fail("Failed to capture a stack trace.");
        }
    }

    void stack_tracer::remove_trace(key a_key)
    {
        auto it = m_traces.find(a_key);
        if(it != m_traces.end())
        {
            m_traces.erase(it);
        }
        else
        {
            JE_fail("Failed to remove a stack trace.");
        }
    }

    void stack_tracer::print_remaining_traces()
    {
        init_symbol_ref();

        JE_printf_ln("###########################################");
        JE_printf_ln("There are [%lld] existing traces in the stack tracker!", get_num_remaining_traces());
        JE_printf("\n");

        size_t num = 1;
        for(auto it = m_traces.begin(); it != m_traces.end(); ++it)
        {
            JE_printf("Trace #%lld:\n", num);
            stack_trace trace = it->second;
            print_trace(trace);
            JE_printf("\n");

            ++num;
        }

        JE_printf_ln("###########################################");

        cleanup_symbol_ref();
    }

    void stack_tracer::print_current_trace()
    {
        stack_tracer tracer;
        stack_trace trace{};

        tracer.generate_trace(trace);

        if(trace.m_num_traces > 0)
        {
            JE_printf_ln("###########################################");
            JE_printf_ln("Printing current stack trace: \n");
            print_trace(trace);
            JE_printf_ln("###########################################");
        }
        else
        {
            JE_fail("Failed to capture a stack trace.");
        }
    }

    const char* stack_tracer::trim_file_name(const char* a_absolute_file_name)
    {
        static const char* root_folder_name = "src";
        static const size_t num_chars_to_advance = strlen(root_folder_name) + strlen(util::get_file_separator());
        
        const char* root_in_name = strstr(a_absolute_file_name, root_folder_name);
        if(root_in_name != nullptr)
        {
            return root_in_name + num_chars_to_advance;
        }
        else 
        {
            return a_absolute_file_name;
        }
    }

    void stack_tracer::init_symbol_ref()
    {
        size_t prev_num_refs = s_symbol_reference_num++;
        if(prev_num_refs == 0)
        {
            init_symbols();
        }
    }

    void stack_tracer::cleanup_symbol_ref()
    {
        size_t prev_num_refs = s_symbol_reference_num--;
        if(prev_num_refs == 1)
        {
            cleanup_symbols();
        }
    }

    std::atomic<size_t> stack_tracer::s_symbol_reference_num(0);
}}

#endif
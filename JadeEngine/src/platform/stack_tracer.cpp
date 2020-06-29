#include "stack_tracer.h"

#if JE_USE_STACK_TRACER

namespace je { namespace platform {

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
            JE_printf("Trace #%lld:", num);
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
        // TODO

    }

    const char* stack_tracer::trim_file_name(const char* a_absolute_file_name)
    {
        // TODO
        return a_absolute_file_name;
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
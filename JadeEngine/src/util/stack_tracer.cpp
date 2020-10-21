#include "stack_tracer.h"

#if JE_USE_STACK_TRACER

#include "misc.h"

namespace je { namespace util {

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

        JE_print_ln("###########################################");
        JE_print_ln("There are [%zu] existing traces in the stack tracker!", get_num_remaining_traces());
        JE_print("\n");

        size_t num = 1;
        for(auto it = m_traces.begin(); it != m_traces.end(); ++it)
        {
            JE_print("Trace #%zu:\n", num);
            stack_trace trace = it->second;
            print_trace(trace);
            JE_print("\n");

            ++num;
        }

        JE_print_ln("###########################################");

        cleanup_symbol_ref();
    }

    void stack_tracer::print_current_trace()
    {
        stack_tracer tracer;
        stack_trace trace{};

        tracer.generate_trace(trace);

        if(trace.m_num_traces > 0)
        {
            JE_print_ln("###########################################");
            JE_print_ln("Printing current stack trace: \n");
            print_trace(trace);
            JE_print_ln("###########################################");
        }
        else
        {
            JE_fail("Failed to capture a stack trace.");
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
#include "platf/stack_tracer.h"

#if JE_PLATFORM_LINUX
#if JE_USE_STACK_TRACER

#include "platf_linux.h"
#include "util/misc.h"

#include <execinfo.h>

namespace je { namespace platf {

    void stack_tracer::generate_trace(stack_trace& a_out_trace)
    {
        a_out_trace.m_num_traces = backtrace(a_out_trace.m_traces, k_num_frames);
    }

    void stack_tracer::init_symbols()
    {
        // Nothing to do here.
    }

    void stack_tracer::cleanup_symbols()
    {
        // Nothing to do here.
    }

    void stack_tracer::print_trace(stack_trace& a_trace)
    {
        char** names = backtrace_symbols(a_trace.m_traces, a_trace.m_num_traces);
        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            JE_printf("%s\n", names[i]);
        }
    }
}}

#endif
#endif
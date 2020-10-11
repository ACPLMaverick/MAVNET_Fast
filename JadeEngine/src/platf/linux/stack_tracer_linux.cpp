#include "platf/stack_tracer.h"

#if JE_PLATFORM_LINUX
#if JE_USE_STACK_TRACER

#include "platf_linux.h"
#include "util/misc.h"

#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

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
        // Code for translating these symbols to something meaningful took from here:
        // https://gist.github.com/fmela/591333/c64f4eb86037bb237862a8283df70cdfc25f01d3
        
        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            Dl_info info;
            if(dladdr(a_trace.m_traces[i], &info))
            {
                int status;
                char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
                JE_print("%-3zd %p %s + %zd\n",
					 i, static_cast<void*>(a_trace.m_traces[i]),
					 status == 0 ? demangled : info.dli_sname,
					 (char *)a_trace.m_traces[i] - (char *)info.dli_saddr);
                free(demangled);
            }
            else
            {
                JE_print("%-3zd %p\n", i, static_cast<void*>(a_trace.m_traces[i]));
            }
        }
    }
}}

#endif
#endif
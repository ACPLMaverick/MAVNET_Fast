#include "platf/stack_tracer.h"

#if JE_PLATFORM_LINUX
#if JE_USE_STACK_TRACER

#include "platf_linux.h"
#include "platf/platf.h"
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
        /*
        char** names = backtrace_symbols(a_trace.m_traces, a_trace.m_num_traces);

        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            JE_print
            (
                "[%p] : [%s] : [%s] : [%zd]\n",
				static_cast<void*>(a_trace.m_traces[i]),
                names[i],
                "TODO",
                0l
            );
        }

        free(names);
        */
       
        // Code for translating these symbols to something meaningful took from here:
        // https://gist.github.com/fmela/591333/c64f4eb86037bb237862a8283df70cdfc25f01d3
        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            Dl_info info;
            if(dladdr(a_trace.m_traces[i], &info))
            {
                int status;
                char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);

                data::string addr2line = platf::util::call_system_command
                (
                    data::string::format("llvm-addr2line %p", a_trace.m_traces[i])
                );

                data::array<data::string> addr2line_split;
                addr2line.split(":", addr2line_split);

                if(addr2line_split.size() > 1)
                {
                    // Remove newline character at the end of command.
                    addr2line_split[1].trim_end(1);
                }

                JE_print
                (
                    "[%p] : [%s] : [%s] : [%s]\n",
					static_cast<void*>(a_trace.m_traces[i]),
					status == 0 ? demangled : info.dli_sname,
				    addr2line_split.size() > 1 ? addr2line_split[0].get_data() : "??",
                    addr2line_split.size() > 1 ? addr2line_split[1].get_data() : "??"
                );
                free(demangled);
            }
            else
            {
                JE_print("[%p] : Unknown function\n", static_cast<void*>(a_trace.m_traces[i]));
            }
        }
    }
}}

#endif
#endif
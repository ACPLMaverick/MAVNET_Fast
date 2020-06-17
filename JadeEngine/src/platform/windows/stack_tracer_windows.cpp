#include "platform/stack_tracer.h"

#if JE_PLATFORM_WINDOWS
#if JE_USE_STACK_TRACER

#include "platform.h"

#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp")

namespace je { namespace platform {

    void stack_tracer::capture_trace(key a_key)
    {
        stack_trace new_trace;
        
        new_trace.m_num_traces = CaptureStackBackTrace(k_num_frames_to_skip, k_num_frames, new_trace.m_traces, NULL);

        if(new_trace.m_num_traces != 0)
        {
            m_traces.emplace(a_key, new_trace);
        }
        else
        {
            JE_fail("Failed to capture a stack trace.");
        }
    }

    void stack_tracer::init_symbols()
    {
        JE_verify(SymInitialize(GetCurrentProcess(), NULL, TRUE), "Symbol initialization failed.");
    }

    void stack_tracer::cleanup_symbols()
    {
        JE_verify(SymCleanup(GetCurrentProcess()), "Symbol cleanup failed.");
    }

    void stack_tracer::print_trace(stack_trace& a_trace)
    {
        const HANDLE process = GetCurrentProcess();

        IMAGEHLP_SYMBOL symbol;
        symbol.MaxNameLength = MAX_SYM_NAME;
        symbol.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);

        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            if(SymGetSymFromAddr(process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), 0, &symbol))
            {
                if(SymGetLineFromAddr(process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), 0, &line))
                {
                    JE_printf_ln("[%p] : [%s] : [%s] : [%lu]",
                        a_trace.m_traces[i], symbol.Name, line.FileName, line.LineNumber);
                }
                else
                {
                    JE_printf_ln("[%p] : [%s] : Unknown file", a_trace.m_traces[i], symbol.Name);
                }
            }
            else
            {
                JE_printf_ln("[%p] : Unknown function", a_trace.m_traces[i]);
            }
        }
    }

    std::atomic<size_t> stack_tracer::s_symbol_reference_num(0);
}}

#endif
#endif
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
        
        // TODO Use this:
        // https://stackoverflow.com/questions/22467604/how-can-you-use-capturestackbacktrace-to-capture-the-exception-stack-not-the-ca
        new_trace.m_num_traces = CaptureStackBackTrace(k_num_frames_to_skip, k_num_frames, new_trace.m_traces, NULL);

        if(new_trace.m_num_traces != 0)
        {
            init_symbol_ref();
            print_trace(new_trace);
            cleanup_symbol_ref();
            m_traces.emplace(a_key, new_trace);
        }
        else
        {
            JE_fail("Failed to capture a stack trace.");
        }
    }

    void stack_tracer::init_symbols()
    {
        const HANDLE process = GetCurrentProcess();
        JE_verify(SymInitialize(process, NULL, FALSE), "Symbol initialization failed.");
        SymSetOptions(SYMOPT_LOAD_LINES);
    }

    void stack_tracer::cleanup_symbols()
    {
        const HANDLE process = GetCurrentProcess();
        JE_verify(SymCleanup(process), "Symbol cleanup failed.");
    }

    void stack_tracer::print_trace(stack_trace& a_trace)
    {
        const HANDLE process = GetCurrentProcess();
        static char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];

        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
        symbol->MaxNameLen = MAX_SYM_NAME;
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);

        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            if(SymFromAddr(process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), 0, symbol))
            {
                if(SymGetLineFromAddr(process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), 0, &line))
                {
                    JE_printf_ln("[%p] : [%s] : [%s] : [%lu]",
                        a_trace.m_traces[i], symbol->Name, line.FileName, line.LineNumber);
                }
                else
                {
                    print_last_error();
                    JE_printf_ln("[%p] : [%s] : Unknown file", a_trace.m_traces[i], symbol->Name);
                }
            }
            else
            {
                print_last_error();
                JE_printf_ln("[%p] : Unknown function", a_trace.m_traces[i]);
            }
        }
    }

    std::atomic<size_t> stack_tracer::s_symbol_reference_num(0);
}}

#endif
#endif
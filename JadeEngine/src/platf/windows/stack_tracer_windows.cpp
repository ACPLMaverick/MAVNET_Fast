#include "platf/stack_tracer.h"

#if JE_PLATFORM_WINDOWS
#if JE_USE_STACK_TRACER

#include "platf_windows.h"
#include "util/misc.h"

#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp")

namespace je { namespace platf {

    static struct
    {
        HANDLE process{0};
        DWORD64 symbol_module{0};
    } g_symbol_data;

    void stack_tracer::generate_trace(stack_trace& a_out_trace)
    {
        a_out_trace.m_num_traces = CaptureStackBackTrace(0, k_num_frames, a_out_trace.m_traces, NULL);
    }

    void stack_tracer::init_symbols()
    {
        JE_verify(DuplicateHandle(
            GetCurrentProcess(),
            GetCurrentProcess(),
            GetCurrentProcess(),
            &g_symbol_data.process,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        ), "DuplicateHandle failed.");

        TCHAR module_name[MAX_PATH];

        JE_verify(GetModuleFileName(NULL, module_name, MAX_PATH), "GetModuleFilename failed.");

        JE_verify(SymInitialize(g_symbol_data.process, module_name, TRUE), "Symbol initialization failed.");
        SymSetOptions(SYMOPT_LOAD_LINES);

        g_symbol_data.symbol_module = SymLoadModule64(g_symbol_data.process, NULL, module_name, NULL, 0, 0);
        JE_assert(g_symbol_data.symbol_module != 0, "SymLoadModule64 failed.");
    }

    void stack_tracer::cleanup_symbols()
    {
        JE_assert(g_symbol_data.process != 0 && g_symbol_data.symbol_module != 0, "Cleaning up uninitialized symbols.");
        JE_verify(SymUnloadModule64(g_symbol_data.process, g_symbol_data.symbol_module), "SymUnloadModule64 failed.");
        JE_verify(SymCleanup(g_symbol_data.process), "Symbol cleanup failed.");

        ZeroMemory(&g_symbol_data, sizeof(g_symbol_data));
    }

    void stack_tracer::print_trace(stack_trace& a_trace)
    {
        JE_assert(g_symbol_data.process != 0, "Uninitialized symbols");

        static char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];

        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
        symbol->MaxNameLen = MAX_SYM_NAME;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

        DWORD64 displacement;

        for(size_t i = 0; i < a_trace.m_num_traces; ++i)
        {
            displacement = 0;
            if(SymFromAddr(g_symbol_data.process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), &displacement, symbol))
            {
                DWORD disp(displacement);
                if(SymGetLineFromAddr(g_symbol_data.process, reinterpret_cast<DWORD64>(a_trace.m_traces[i]), &disp, &line))
                {
                    JE_printf_ln("[%p] : [%s] : [%s] : [%lu]",
                        a_trace.m_traces[i], symbol->Name, je::util::misc::trim_file_name(line.FileName), line.LineNumber);
                }
                else
                {
                    util::print_last_error();
                    JE_printf_ln("[%p] : [%s] : Unknown file", a_trace.m_traces[i], symbol->Name);
                }
            }
            else
            {
                util::print_last_error();
                JE_printf_ln("[%p] : Unknown function", a_trace.m_traces[i]);
            }
        }
    }
}}

#endif
#endif
#pragma once

#include "global.h"

#if JE_CONFIG_DEBUG
#define JE_USE_STACK_TRACER 1
#else
#define JE_USE_STACK_TRACER 0
#endif

#if JE_USE_STACK_TRACER

#include <unordered_map>
#include <atomic>

namespace je { namespace platform {

    class stack_tracer
    {
    public:

        using key = void*;

        stack_tracer();
        ~stack_tracer();

        void clear();

        void capture_trace(key a_key);
        void remove_trace(key a_key);

        size_t get_num_remaining_traces() const { return m_traces.size(); }
        void print_remaining_traces();

    private:

        using stack_ptr = void*;
        static const size_t k_num_total_frames = 36;
        static const size_t k_num_frames_to_skip = 4;
        static const size_t k_num_frames = k_num_total_frames - k_num_frames_to_skip;

        struct stack_trace
        {
            stack_ptr m_traces[k_num_frames];
            size_t m_num_traces;
        };

        static void init_symbols();
        static void cleanup_symbols();

        void init_symbol_ref();
        void cleanup_symbol_ref();

        void print_trace(stack_trace& trace);

        std::unordered_map<key, stack_trace> m_traces;

        static std::atomic<size_t> s_symbol_reference_num;
    };

}}

#endif
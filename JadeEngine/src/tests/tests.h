#pragma once

#include "global.h"

namespace je 
{
    class engine;
}

namespace je { namespace tests {

    class tester
    {
    public:

        static void run();

    private:

        static void test_mem();
        static void test_stack_tracer();
        static void test_collections();
        static void test_string();
        static void test_allocator_state_print();
        static void test_math();
        static void test_thread();
        static void test_object_pool();
    };

}}
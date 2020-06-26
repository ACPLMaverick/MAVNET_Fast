#pragma once

#include "global.h"

namespace je { namespace tests {

    class tester
    {
    public:

        static void run();

    private:

        static void test_mem();
        static void test_stack_tracer();
        static void test_memory_access_guard();

    };

}}
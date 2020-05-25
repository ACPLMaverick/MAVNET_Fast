#include "global.h"

#include "mem/system_allocator.h"
#include "mem/linear_allocator.h"

#include <iostream>

namespace je {

    void test_alloc()
    {
        mem::system_allocator allocator;
        mem::linear_allocator linear_allocator(allocator, 512);

        size_t num_bytes = 64;
        while(num_bytes < 512)
        {
            linear_allocator.allocate(num_bytes);
            num_bytes *= 2;
        }

        linear_allocator.clear();

        JE_printf_ln("Allocation test succeeded");
    }

}

int main()
{
    je::test_alloc();
    return 0;
}
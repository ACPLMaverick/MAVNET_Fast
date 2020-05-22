#include "global.h"

#include "mem/system_allocator.h"

#include <iostream>

namespace je {

    void test_config()
    {
#if JE_CONFIG_DEBUG
        std::cout << "Hello world! Debug.";
#elif JE_CONFIG_PROFILE
        std::cout << "Hello world! Profile.";
#elif JE_CONFIG_RELEASE
        std::cout << "Hello world! Release.";
#else
    #error "Configuration not specified."
#endif
    }

    void test_alloc()
    {
        mem::system_allocator allocator;
        void* mem = allocator.allocate(64);
        allocator.free(mem);
    }

}

int main()
{
    //test_config();
    je::test_alloc();
    return 0;
}
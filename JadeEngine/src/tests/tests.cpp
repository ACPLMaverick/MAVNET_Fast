#include "tests.h"

#include "mem/system_allocator.h"
#include "mem/linear_allocator.h"
#include "mem/stack_allocator.h"

namespace je { namespace tests {

    void tester::run()
    {
        test_mem();
    }

    void* mem_alloc_and_fill(mem::base_allocator& allocator, size_t size, 
        uint8_t byte, mem::alignment al = mem::base_allocator::k_default_alignment)
    {
        void* mem = allocator.allocate(size, al);
        if(mem != nullptr)
        {
            memset(mem, byte, size);
        }

        return mem;
    }

    void mem_free_and_check(mem::base_allocator& allocator, void* mem, 
        size_t size, uint8_t byte)
    {
        uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(mem);

        for(size_t i = 0; i < size; ++i)
        {
            if(byte_ptr[i] != byte)
            {
                JE_fail("Memory has been corrupted!");
                break;
            }
        }

        allocator.free(mem);
    }

    void tester::test_mem()
    {
        mem::system_allocator allocator;

        {
            mem::linear_allocator linear_allocator(allocator, 512);

            size_t num_bytes = 4;
            while(num_bytes < 128)
            {
                void* memory = linear_allocator.allocate(num_bytes, mem::alignment::k_16);
                JE_unused(memory);
                num_bytes *= 2;
            }

            linear_allocator.clear();
        }

        {
            mem::stack_allocator stack_allocator(allocator, 128);

            void* mem_1 = mem_alloc_and_fill(stack_allocator, 32, 0xAB);
            void* mem_2 = mem_alloc_and_fill(stack_allocator, 53, 0xCD);

            mem_free_and_check(stack_allocator, mem_2, 53, 0xCD);
            
            mem_alloc_and_fill(stack_allocator, 40, 0xEF);
            void* mem_3 = mem_alloc_and_fill(stack_allocator, 12, 0x12,
                je::mem::alignment::k_0);

            mem_free_and_check(stack_allocator, mem_3, 12, 0x12);
            mem_free_and_check(stack_allocator, mem_2, 40, 0xEF);
            mem_free_and_check(stack_allocator, mem_1, 32, 0xAB);

            mem::stack_mem st_mem_1 = stack_allocator.allocate_stack_mem(16);
            mem::stack_mem st_mem_2 = stack_allocator.allocate_stack_mem(16);
            mem::stack_mem st_mem_3 = stack_allocator.allocate_stack_mem(32);
        }

        JE_printf_ln("Allocation test passed.");
    }

}}
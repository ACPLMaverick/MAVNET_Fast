#include "tests.h"

#include "engine.h"

#include "mem/system_allocator.h"
#include "mem/linear_allocator.h"
#include "mem/stack_allocator.h"
#include "mem/general_purpose_allocator.h"
#include "mem/pool_allocator.h"

#include "data/data.h"

namespace je { namespace tests {

    JE_bitfield test_bitfield_enum
    {
        k_val_0 = (1 << 0),
        k_val_1 = (1 << 1),
        k_val_2 = (1 << 2),
        k_num_bits = 3,
        k_all = 0xFF
    };

    void tester::run()
    {
        test_mem();
        //test_stack_tracer();

        m_engine = new engine();

        test_collections();
        test_string();
        test_math();
        test_thread();

        JE_safe_delete(m_engine);
    }

    class mem_tester
    {
    public:
        mem_tester(mem::base_allocator& a_allocator, size_t a_num_bytes, 
            uint8_t a_byte_to_fill, mem::alignment a_al = mem::base_allocator::k_default_alignment)
            : m_allocator(a_allocator)
            , m_mem(m_allocator.allocate(a_num_bytes, a_al))
            , m_num_bytes(a_num_bytes)
            , m_byte_to_fill(a_byte_to_fill)
        {
            if(m_mem != nullptr)
            {
                memset(m_mem, m_byte_to_fill, m_num_bytes);
            }
        }

        ~mem_tester()
        {
            uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(m_mem);

            for(size_t i = 0; i < m_num_bytes; ++i)
            {
                if(byte_ptr[i] != m_byte_to_fill)
                {
                    JE_fail("Memory has been corrupted!");
                    break;
                }
            }

            m_allocator.free(m_mem);
        }

        void* get() { return m_mem; }

    private:

        mem::base_allocator& m_allocator;
        void* m_mem;
        size_t m_num_bytes;
        uint8_t m_byte_to_fill;
    };

    void tester::test_mem()
    {
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
            }

            {
                mem::stack_allocator stack_allocator(allocator, 256);

                {
                    mem_tester mem_1(stack_allocator, 32, 0xAB);
                    {
                        mem_tester mem_2(stack_allocator, 53, 0xCD);
                    }
                    
                    mem_tester mem_2(stack_allocator, 40, 0xEF);
                    mem_tester mem_3(stack_allocator, 12, 0x12, je::mem::alignment::k_0);
                }

                mem::stack_mem st_mem_1 = stack_allocator.allocate_stack_mem(16);
                mem::stack_mem st_mem_2 = stack_allocator.allocate_stack_mem(16);
                mem::stack_mem st_mem_3 = stack_allocator.allocate_stack_mem(32);
            }

            {
                mem::general_purpose_allocator gp_allocator(allocator, 300);

                {
                    mem_tester mem_1(gp_allocator, 32, 0xAB);
                    {
                        mem_tester* mem_2 = new mem_tester(gp_allocator, 53, 0xCD, je::mem::alignment::k_4);
                        mem_tester* mem_3 = new mem_tester(gp_allocator, 40, 0xEF);
                        mem_tester* mem_4 = new mem_tester(gp_allocator, 12, 0x12, je::mem::alignment::k_0);

                        delete mem_3;
                        delete mem_4;
                        delete mem_2;
                    }
                    
                    mem_tester* mem_2 = new mem_tester(gp_allocator, 128, 0xEF);
                    mem_tester* mem_3 = new mem_tester(gp_allocator, 32, 0x12, je::mem::alignment::k_0);

                    JE_unused(mem_2);
                    delete mem_2;
                    delete mem_3;
                }
            }

            {
                static const size_t obj_num_bytes = 32;
                mem::pool_allocator pool_allocator(allocator, obj_num_bytes, 8);

                {
                    mem_tester mem_1(pool_allocator, obj_num_bytes, 0xAB);
                }
                {
                    mem_tester* mem_1 = new mem_tester(pool_allocator, obj_num_bytes, 0xAB);
                    mem_tester* mem_2 = new mem_tester(pool_allocator, obj_num_bytes, 0xBC);
                    mem_tester* mem_3 = new mem_tester(pool_allocator, obj_num_bytes, 0xCD);
                    mem_tester* mem_4 = new mem_tester(pool_allocator, obj_num_bytes, 0xEF);

                    delete mem_2;
                    delete mem_3;

                    {
                        mem_tester* mem_5 = new mem_tester(pool_allocator, obj_num_bytes, 0xFE);
                        mem_tester* mem_6 = new mem_tester(pool_allocator, obj_num_bytes, 0xDC);
                        mem_tester* mem_7 = new mem_tester(pool_allocator, obj_num_bytes, 0xCB);
                        mem_tester* mem_8 = new mem_tester(pool_allocator, obj_num_bytes, 0xBA);

                        delete mem_6;
                        delete mem_5;
                        delete mem_7;
                        delete mem_8;
                    }

                    delete mem_1;
                    delete mem_4;
                }
            }
        }

        JE_printf_ln("Allocator test passed.");
    }

    void tester::test_stack_tracer()
    {
        mem::system_allocator allocator;
        void* mem = allocator.allocate(64);
        JE_unused(mem);
        // Forget to free this memory.
    }

    void tester::test_collections()
    {
        // Array test.
        {
            je::data::static_array<int32_t, 16> small_array;
            small_array[0] = 1;
            small_array[5] = 2;
            for(size_t i = 0; i < small_array.k_num_objects; ++i)
            {
                small_array[i] = small_array.k_num_objects - i;
            }
            //small_array[32] = 8;

            je::data::static_array<int32_t, 32> big_array(small_array, 0);

            je::data::static_bit_array<6> static_bit_array;
            static_bit_array.set(1, true);
            static_bit_array.set(4, true);
            JE_assert(static_bit_array.get(1) == true && static_bit_array.get(2) == false && static_bit_array.get(4) == true, "Assertion failed.");

            je::data::static_bitfield_array<test_bitfield_enum> static_bitfield_array;
            static_bitfield_array.set(test_bitfield_enum::k_val_1, true);
            static_bitfield_array.set(test_bitfield_enum::k_val_2, false);
            static_bitfield_array.set(test_bitfield_enum::k_val_1 | test_bitfield_enum::k_val_0, true);
            JE_assert(static_bitfield_array.get(test_bitfield_enum::k_val_1) == true && static_bitfield_array.get(test_bitfield_enum::k_val_2) == false, "Assertion failed.");
        }

        // Array test
        {
            je::data::array<float> float_arr;
            float_arr.push_back(4.0f);
            float_arr.push_back(3.0f);

            for(size_t i = 0; i < 2148; ++i)
            {
                float_arr.push_back(static_cast<float>(i));
            }

            je::data::array<float> float_arr_2(float_arr);

            for(size_t i = 0; i < 512; ++i)
            {
                float_arr_2.erase(float_arr_2.begin());
            }

            float_arr = float_arr_2;

            /*
            for(const float& flt : float_arr)
            {
                JE_printf_ln("%f ", flt);
            }
            JE_printf("\n");
            */
        }

        /*
        // Stack test
        {
            JE_todo();
        }

        // Queue test
        {
            JE_todo();
        }

        // Set test
        {
            JE_todo();
        }

        // Dict test
        {
            JE_todo();
        }
        */
        JE_printf_ln("Collection test passed.");
    }

    void tester::test_string()
    {
        je::data::string str("Janusz");
        str.append("Wanusz");
        JE_printf_ln("String test passed.");
    }

    void tester::test_math()
    {
        JE_todo();
    }

    void tester::test_thread()
    {
        JE_todo();
    }

    je::engine* tester::m_engine(nullptr);
}}
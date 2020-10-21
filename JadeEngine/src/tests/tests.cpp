#include "tests.h"

#include "engine.h"

#include "mem/mem_manager.h"
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

    void tester::run()
    {
        test_mem();
        //test_stack_tracer();
        test_collections();
        test_string();
        test_allocator_state_print();
        test_object_pool();
        test_math();
        test_thread();
    }

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
                    mem_tester mem_3(stack_allocator, 12, 0x12, je::mem::alignment::k_1);
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
                        mem_tester* mem_4 = new mem_tester(gp_allocator, 12, 0x12, je::mem::alignment::k_1);

                        delete mem_3;
                        delete mem_4;
                        delete mem_2;
                    }
                    
                    mem_tester* mem_2 = new mem_tester(gp_allocator, 128, 0xEF);
                    mem_tester* mem_3 = new mem_tester(gp_allocator, 32, 0x12, je::mem::alignment::k_1);

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

        JE_print_ln("Allocator test passed.");
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
            data::static_array<int32_t, 16> small_array;
            small_array[0] = 1;
            small_array[5] = 2;
            for(size_t i = 0; i < small_array.k_num_objects; ++i)
            {
                small_array[i] = small_array.k_num_objects - i;
            }
            //small_array[32] = 8;

            data::static_array<int32_t, 32> big_array(small_array, 0);

            data::static_bit_array<6> static_bit_array;
            static_bit_array.set(1, true);
            static_bit_array.set(4, true);
            JE_assert(static_bit_array.get(1) == true && static_bit_array.get(2) == false && static_bit_array.get(4) == true);

            data::static_bitfield_array<test_bitfield_enum> static_bitfield_array;
            static_bitfield_array.set(test_bitfield_enum::k_val_1, true);
            static_bitfield_array.set(test_bitfield_enum::k_val_2, false);
            static_bitfield_array.set(test_bitfield_enum::k_val_1 | test_bitfield_enum::k_val_0, true);
            JE_assert(static_bitfield_array.get(test_bitfield_enum::k_val_1) == true && static_bitfield_array.get(test_bitfield_enum::k_val_2) == false);
        }

        // Array test
        {
            data::array<float> float_arr;
            float_arr.push_back(4.0f);
            float_arr.push_back(3.0f);

            for(size_t i = 0; i < 2148; ++i)
            {
                float_arr.push_back(static_cast<float>(i));
            }

            data::array<float> float_arr_2(float_arr);

            for(size_t i = 0; i < 512; ++i)
            {
                float_arr_2.erase(float_arr_2.begin());
            }

            float_arr = float_arr_2;

            /*
            for(const float& flt : float_arr)
            {
                JE_print_ln("%f ", flt);
            }
            JE_print("\n");
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
        JE_print_ln("Collection test passed.");
    }

    void tester::test_string()
    {
        data::string fmtted_str(data::string::format("%s %s %d %d %d", "Hello", "World", 1, 2, 3));
        JE_assert(fmtted_str == "Hello World 1 2 3");

        data::string int_str(data::string::from_int64(-783246));
        JE_assert(int_str == "-783246");

        data::string flt_str(data::string::from_float(-783.123456, 2));
        JE_assert(flt_str == "-783.12");

        data::string flt_str_2(data::string::from_float(-783.123456));
        //JE_assert(flt_str_2 == "-783.123456");

        const int32_t int_parsed = data::string::parse_int32("321");
        JE_assert(int_parsed == 321);

        const float flt_parsed = data::string::parse_float("0.321");
        JE_assert(flt_parsed == 0.321f);

        data::string starts = "Starts With";
        JE_assert(starts.is_starting_with("Sta"));
        JE_assert(starts.is_starting_with("Stanope") == false);
        JE_assert(starts.is_starting_with("Starts With Something") == false);
        JE_assert(starts.is_ending_with("With"));
        JE_assert(starts.is_ending_with("Wit") == false);
        JE_assert(starts.is_ending_with("It Starts With") == false);

        data::string str("Jonah");
        JE_assert(str == "Jonah");
        str.append("Smith");
        JE_assert(str == "JonahSmith");

        data::string zdz("Zdzislaw");
        zdz.insert("Stary");
        JE_assert(zdz == "ZdzislawStary");
        zdz.insert("Bardzo", 8);
        JE_assert(zdz == "ZdzislawBardzoStary");
        zdz.insert("Trzeci Wielki Pan", 8, 6, 13);
        JE_assert(zdz == "Zdzislaw Wielki BardzoStary");
        JE_assert(zdz.get_data()[zdz.get_size()] == 0);

        data::string sna("Snakerton");
        sna.replace("Eater", 5);
        JE_assert(sna == "SnakeEater");

        data::string liq("LiquidExquisiteSnake");
        liq.replace("RevolverOcelotHere", 6, 14, 8, 13);
        JE_assert(liq == "LiquidOcelotSnake");

        data::string run("Run The Shop");
        run.erase(3, 6);
        JE_assert(run == "Run Shop");
        size_t run_size = run.get_size();
        JE_assert(run_size == 8);
        run.erase(3);
        JE_assert(run == "Run");
        run_size = run.get_size();
        JE_assert(run_size == 3);

        data::string tof("Will The Replace The Strings From The Here");
        tof.find_and_replace(" The ", " ");
        JE_assert(tof == "Will Replace Strings From Here");

        data::string era("Erase Spaces Please ");
        era.find_and_replace(" ", "");
        JE_assert(era == "EraseSpacesPlease");

        data::string swap_a("Toad");
        data::string swap_b("Auld Lang Syne");
        swap_a.swap(swap_b);
        JE_assert(swap_a == "Auld Lang Syne");
        JE_assert(swap_b == "Toad");

        data::string sub("This Is Sparta");
        sub.substring(5, 6);
        JE_assert(sub == "Is");

        data::string trm("TrimMeHard");
        trm.trim_front(4);
        JE_assert(trm == "MeHard");
        trm.trim_end(4);
        JE_assert(trm == "Me");

        data::string substr_construct = data::string::from_substring("YeahRightNow", 4, 8);
        JE_assert(substr_construct == "Right");

        data::string splt(" This We Will Split On Spaces ");
        data::array<data::string> splt_splt;
        splt.split(" ", splt_splt);
        JE_assert(splt_splt.size() == 6);
        JE_assert(splt_splt[0] == "This");
        JE_assert(splt_splt[1] == "We");
        JE_assert(splt_splt[2] == "Will");
        JE_assert(splt_splt[3] == "Split");
        JE_assert(splt_splt[4] == "On");
        JE_assert(splt_splt[5] == "Spaces");
        splt_splt.clear();
        splt.split("Will", splt_splt);
        JE_assert(splt_splt.size() == 2);
        JE_assert(splt_splt[0] == " This We ");
        JE_assert(splt_splt[1] == " Split On Spaces ");

        data::string case_test("this is sparta");
        data::string case_upper(case_test);
        case_upper.to_upper_case();
        JE_assert(case_upper == "THIS IS SPARTA");
        JE_assert(case_upper.is_upper_case());
        JE_assert(case_upper.is_lower_case() == false);
        case_upper.to_lower_case();
        JE_assert(case_upper == case_test);
        
        case_test = "43214this*()*Is122sparta";
        case_test.to_capitalized_case();
        JE_assert(case_test == "43214This*()*Is122Sparta");

        case_test = "a simple example";
        case_test.to_capitalized_case();
        JE_assert(case_test == "A Simple Example");
        JE_assert(case_test.is_upper_case() == false);
        JE_assert(case_test.is_lower_case() == false, "String [%s] is lower case!", case_test.get_data());

        JE_print_ln("String test passed.");
    }

    void tester::test_allocator_state_print()
    {
        {
            data::string str_a("aaaaaaaa");
            data::string str_b("bbbbbbbbbbbbbbbbbbbbbbbbb");
            data::string str_c("c");

            data::array<tester> testers(50);
            data::queue<size_t> qu;
            for(size_t i = 0; i < 100; ++i)
            {
                qu.push_back(i);
            }
            for(size_t i = 0; i < 22; ++i)
            {
                str_c += str_c;
            }

            engine::get_mem_manager().print_memory_summary();
        }
        JE_print_ln("After deallocation...");
        engine::get_mem_manager().print_memory_summary();
    }

    void tester::test_object_pool()
    {
        JE_todo();
    }

    void tester::test_math()
    {
        JE_todo();
    }

    void tester::test_thread()
    {
        JE_todo();
    }
}}
#pragma once

#include "static_array.h"

namespace je { namespace data {

    template <typename object_type, size num_objects>
    class static_stack : public static_array<object_type, num_objects>
    {
    public:

        using base = static_array<object_type, num_objects>;
        using base::static_array;

        static_stack(const object_type a_arr[])
            : base(a_arr)
            , m_current_size(base::k_num_objects)
        {
        }

        static_stack(std::initializer_list<object_type> a_list)
        {
            copy(a_list.begin(), a_list.size());
            m_current_size = a_list.size();
        }

        static_stack(const object_type& a_value_to_push, size a_num_to_push = 1)
        {
            for(size i = 0; i < a_num_to_push; ++i)
            {
                push(a_value_to_push);
            }
        }

        static_stack(const static_stack& a_other)
            : base(a_other)
            , m_current_size(a_other.m_current_size)
        {
        }

        // Can return null if stack size is exceeded.
        object_type* push_uninitialized()
        {
            if(m_current_size < base::k_num_objects)
            {
                object_type* to_ret = &base::m_array[m_current_size];
                ++m_current_size;
                return to_ret;
            }
            else
            {
                JE_fail("Static stack max size [%zd] exceeded.", base::k_num_objects);
                return nullptr;
            }
        }

        bool push(const object_type& a_obj)
        {
            object_type* ptr = push_uninitialized();
            if(ptr == nullptr)
            {
                return false;
            }

            *ptr = a_obj;
            return true;
        }

        template<typename... var_args>
        bool push(var_args... a_args)
        {
            object_type* ptr = push_uninitialized();
            if(ptr == nullptr)
            {
                return false;
            }

            new (ptr) object_type(a_args...);
            return true;
        }

        bool pop()
        {
            if(m_current_size > 0)
            {
                base::m_array[m_current_size - 1].~object_type();
                --m_current_size;
            }
            else
            {
                JE_fail("Static stack is empty.");
                return false;
            }
        }

        size get_size() const
        {
            return m_current_size;
        }

        void clear()
        {
            m_current_size = 0;
        }

    private:

        // TODO atomic.
        size m_current_size = 0;
    };

}}
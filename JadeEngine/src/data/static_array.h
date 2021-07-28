#pragma once

#include "global.h"

namespace je { namespace data {

    template <typename object_type, size num_objects>
    class static_array
    {
    public:

        static const size k_num_objects = num_objects;

        static_array()
        {
        }

        static_array(const object_type arr[])
        {
            copy<num_objects>(arr);
        }

        static_array(std::initializer_list<object_type> list)
        {
            JE_assert(list.size() == k_num_objects);
            copy<num_objects>(list.begin());
        }

        static_array(const object_type& a_value_to_copy_over)
        {
            for(size i = 0; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        static_array(const static_array& a_other)
        {
            copy<num_objects>(a_other.get_data());
        }

        template <size other_num_objects>
        static_array(const static_array<object_type, other_num_objects>& a_other)
        {
            static_assert(num_objects >= other_num_objects, "Trying to copy bigger array into smaller one.");
            copy<other_num_objects>(a_other.get_data());
        }

        template <size other_num_objects>
        static_array(const static_array<object_type, other_num_objects>& a_other, const object_type& a_value_to_copy_over)
            : static_array(a_other)
        {
            for(size i = other_num_objects; i < k_num_objects; ++i)
            {
                m_array[i] = a_value_to_copy_over;
            }
        }

        ~static_array()
        {
        }

        object_type* get_data() { return m_array; }
        const object_type* get_data() const { return m_array; }

        static_array& operator=(const static_array& a_other)
        {
            copy<num_objects>(a_other.m_array);
            return *this;
        }

        object_type& operator[](size a_index)
        {
            JE_assert(a_index < k_num_objects, "Out-of-bounds index.");
            return m_array[a_index];
        }

        const object_type& operator[](size a_index) const
        {
            JE_assert(a_index < k_num_objects, "Out-of-bounds index.");
            return m_array[a_index];
        }

    protected:

        static const size k_copy_threshold_value_bytes = 1024;

        template<size num_objects_to_copy>
        inline void copy(const object_type* a_other_array)
        {
            if(std::is_pod<object_type>() && sizeof(object_type) * num_objects_to_copy > k_copy_threshold_value_bytes)
            {
                std::memcpy(m_array, a_other_array, num_objects_to_copy * sizeof(object_type));
            }
            else
            {
                #pragma unroll
                for(size i = 0; i < num_objects_to_copy; ++i)
                {
                    m_array[i] = a_other_array[i];
                }
            }
        }

        inline void copy(const object_type* a_other_array, size a_num_objects_to_copy)
        {
            if(std::is_pod<object_type>() && sizeof(object_type) * a_num_objects_to_copy > k_copy_threshold_value_bytes)
            {
                std::memcpy(m_array, a_other_array, a_num_objects_to_copy * sizeof(object_type));
            }
            else
            {
                for(size i = 0; i < a_num_objects_to_copy; ++i)
                {
                    m_array[i] = a_other_array[i];
                }
            }
        }

        object_type m_array[k_num_objects];
    };

}}
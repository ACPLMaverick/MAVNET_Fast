#pragma once

#include "global.h"

// TODO make these compiler-independent.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"

namespace je { namespace math {

    template<typename int_type, size num_components>
    class ivec
    {
    public:
        int_type* m_components;    // Essentially an array of an unknown size.
    };

    template<typename int_type>
    class ivec<int_type, 2>
    {
    public:

        ivec(int_type scalar = 0)
            : x(scalar)
            , y(scalar)
        {
        }

        ivec(int_type a_x, int_type a_y)
            : x(a_x)
            , y(a_y)
        {
        }

        ivec(const ivec& other)
            : x(other.x)
            , y(other.y)
        {
        }

    public:

        union
        {
            int_type m_components[2];
            struct
            {
                int_type x;
                int_type y;
            };
            struct
            {
                int_type r;
                int_type g;
            };
        };
    };

    template<typename int_type>
    class ivec<int_type, 3>
    {
    public:

        ivec(int_type scalar = 0)
            : x(scalar)
            , y(scalar)
            , z(scalar)
        {
        }

        ivec(int_type a_x, int_type a_y, int_type a_z)
            : x(a_x)
            , y(a_y)
            , z(a_z)
        {
        }

        ivec(const ivec& other)
            : x(other.x)
            , y(other.y)
            , z(other.z)
        {
        }

    public:

        union
        {
            int_type m_components[3];
            struct
            {
                int_type x;
                int_type y;
                int_type z;
            };
            struct
            {
                int_type r;
                int_type g;
                int_type b;
            };
        };  
    };

    template<typename int_type>
    class ivec<int_type, 4>
    {
    public:

        ivec(int_type scalar = 0)
            : x(scalar)
            , y(scalar)
            , z(scalar)
            , w(scalar)
        {
        }

        ivec(int_type a_x, int_type a_y, int_type a_z, int_type a_w)
            : x(a_x)
            , y(a_y)
            , z(a_z)
            , w(a_w)
        {
        }

        ivec(const ivec& other)
            : x(other.x)
            , y(other.y)
            , z(other.z)
            , w(other.w)
        {
        }

    public:

        union
        {
            int_type m_components[4];
            struct
            {
                int_type x;
                int_type y;
                int_type z;
                int_type w;
            };
            struct
            {
                int_type r;
                int_type g;
                int_type b;
                int_type a;
            };
        };  
    };

#pragma clang diagnostic pop

    using ivec2 = ivec<i32, 2>;
    using ivec3 = ivec<i32, 3>;
    using ivec4 = ivec<i32, 4>;

    using uvec2 = ivec<u32, 2>;
    using uvec3 = ivec<u32, 3>;
    using uvec4 = ivec<u32, 4>;

    using ivec16_2 = ivec<i16, 2>;
    using ivec16_3 = ivec<i16, 3>;
    using ivec16_4 = ivec<i16, 4>;

    using uvec16_2 = ivec<u16, 2>;
    using uvec16_3 = ivec<u16, 3>;
    using uvec16_4 = ivec<u16, 4>;

    using ivec8_2 = ivec<i8, 2>;
    using ivec8_3 = ivec<i8, 3>;
    using ivec8_4 = ivec<i8, 4>;

    using uvec8_2 = ivec<u8, 2>;
    using uvec8_3 = ivec<u8, 3>;
    using uvec8_4 = ivec<u8, 4>;

    using screen_size = uvec16_2;
    using screen_rect = uvec16_4;

}}
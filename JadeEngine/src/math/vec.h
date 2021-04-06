#pragma once

#include "global.h"
#include "sc.h"


// TODO make these compiler-independent.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"

namespace je { namespace math {

    template<size_t num_components>
    class vec_base
    {
    public:
        float* m_components;    // Essentially an array of an unknown size.
    };

    template<>
    class vec_base<2>
    {
    public:

        vec_base(float scalar = 0.0f)
            : x(scalar)
            , y(scalar)
        {
        }

        vec_base(float a_x, float a_y)
            : x(a_x)
            , y(a_y)
        {
        }

        vec_base(const vec_base& other)
            : x(other.x)
            , y(other.y)
        {
        }

        static float cross(const vec_base& a, const vec_base& b)
        {
            return a.x * b.y - a.y * b.x;
        }

    public:

        union
        {
            float m_components[2];
            struct
            {
                float x;
                float y;
            };
            struct
            {
                float r;
                float g;
            };
        };  
    };

    template<>
    class vec_base<3>
    {
    public:

        vec_base(float scalar = 0.0f)
            : x(scalar)
            , y(scalar)
            , z(scalar)
        {
        }

        vec_base(float a_x, float a_y, float a_z)
            : x(a_x)
            , y(a_y)
            , z(a_z)
        {
        }

        vec_base(const vec_base& other)
            : x(other.x)
            , y(other.y)
            , z(other.z)
        {
        }

        static vec_base cross(const vec_base& a, const vec_base& b)
        {
            return vec_base
                (
                    a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x
                );
        }

    public:

        union
        {
            float m_components[3];
            struct
            {
                float x;
                float y;
                float z;
            };
            struct
            {
                float r;
                float g;
                float b;
            };
        };  
    };

    template<>
    class vec_base<4>
    {
    public:

        vec_base(float scalar = 0.0f)
            : x(scalar)
            , y(scalar)
            , z(scalar)
            , w(scalar)
        {
        }

        vec_base(float a_x, float a_y, float a_z, float a_w)
            : x(a_x)
            , y(a_y)
            , z(a_z)
            , w(a_w)
        {
        }

        vec_base(const vec_base& other)
            : x(other.x)
            , y(other.y)
            , z(other.z)
            , w(other.w)
        {
        }

    public:

        union
        {
            float m_components[4];
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };
            struct
            {
                float r;
                float g;
                float b;
                float a;
            };
        };  
    };

    template<size_t num_components>
    class vec : public interp<vec<num_components>>, public vec_base<num_components>
    {
    public:
        static const constexpr size_t k_num_components = num_components;

        using vec_base<num_components>::vec_base;

        vec(const vec_base<num_components>& upper)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                vec_base<num_components>::m_components[i] = upper.m_components[i];
            }
        }

        template<size_t other_num_components>
        vec(const vec<other_num_components>& other, float value_to_fill = 1.0f)
        {
            constexpr const size_t num_to_fill = num_components > other_num_components ? (num_components - other_num_components) : 0;
            #pragma unroll
            for(size_t i = 0; i < num_components && i < other_num_components; ++i)
            {
                vec_base<num_components>::m_components[i] = other.m_components[i];
            }

            #pragma unroll
            for(size_t i = 0; i < num_to_fill; ++i)
            {
                vec_base<num_components>::m_components[num_components + i] = value_to_fill;
            }
        }

        inline float& operator[](size_t index)
        {
            JE_assert(index < k_num_components, "Invalid index.");
            return get_components()[index];
        }

        inline float operator[](size_t index) const
        {
            JE_assert(index < k_num_components, "Invalid index.");
            return get_components()[index];
        }

        // TODO conversion vec<n> -> vec<m>.

        // TODO add NAN checks.

        float get_length_squared() const
        {
            float sum = 0.0f;
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                sum += get_components()[i] * get_components()[i];
            }
            return sum;
        }

        float get_length() const
        {
            return sc::sqrt(get_length_squared());
        }

        void normalize()
        {
            const float length = get_length();
            JE_assert(sc::is_almost_zero(length) == false, "Normalizing zero-length vector.");

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                get_components()[i] /= length;
            }
        }

        vec get_normalized() const
        {
            vec vec(*this);
            vec.normalize();
            return vec;
        }

        bool operator==(const vec& other) const
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                if(get_components()[i] != other.get_components()[i])
                {
                    return false;
                }
            }

            return true;
        }

        static bool is_almost_equal(const vec& a, const vec& b)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                if(sc::is_almost_equal(a.get_components()[i], b.get_components()[i]) == false)
                {
                    return false;
                }
            }

            return true;
        }

        // TODO get_rotation_from_axis

        static float dot(const vec& a, const vec& b)
        {
            float value = 0.0f;
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                value += a.get_components()[i] * b.get_components()[i];
            }
            return value;
        }

        vec operator+(const vec& other) const
        {
            vec value;

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                value.get_components()[i] = get_components()[i] + other.get_components()[i];
            }

            return value;
        }

        vec operator-(const vec& other) const
        {
            vec value;

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                value.get_components()[i] = get_components()[i] - other.get_components()[i];
            }

            return value;
        }

        vec& operator+=(const vec& other)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                get_components()[i] += other.get_components()[i];
            }
            return *this;
        }

        vec& operator-=(const vec& other)
        {
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                get_components()[i] -= other.get_components()[i];
            }
            return *this;
        }

        vec operator*(const float scalar) const
        {
            JE_math_check_val(scalar);

            vec value;

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                value.get_components()[i] = get_components()[i] * scalar;
            }

            return value;
        }

        vec operator/(const float scalar) const
        {
            JE_math_check_val(scalar);
            JE_assert(sc::is_almost_zero(scalar) == false, "Zero-division.");

            vec value;

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                value.get_components()[i] = get_components()[i] / scalar;
            }

            return value;
        }

        vec& operator*=(float scalar)
        {
            JE_math_check_val(scalar);

            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                get_components()[i] *= scalar;
            }
            return *this;
        }

        vec& operator/=(float scalar)
        {
            JE_math_check_val(scalar);
            JE_assert(sc::is_almost_zero(scalar) == false, "Zero-division.");
            #pragma unroll
            for(size_t i = 0; i < k_num_components; ++i)
            {
                get_components()[i] /= scalar;
            }
            return *this;
        }

    protected:

        inline float* get_components()
        {
            return vec_base<num_components>::m_components;
        }

        inline const float* get_components() const
        {
            return vec_base<num_components>::m_components;
        }
    };

    using vec2 = vec<2>;
    using vec3 = vec<3>;
    using vec4 = vec<4>;
}}

#pragma clang diagnostic pop
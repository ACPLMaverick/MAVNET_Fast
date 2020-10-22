#pragma once

#include "global.h"
#include "constants.h"

#include <cmath>

#define JE_MATH_CHECK_NAN 1 && (JE_CONFIG_DEBUG)

#if JE_MATH_CHECK_NAN
#define JE_math_check_val(_val_) je::math::scalar::check_val(_val_)
#else
#define JE_math_check_val(_val_)
#endif

namespace je { namespace math {

    class scalar
    {
    public:

        // MinMax
        
        template<typename num_type>
        static inline num_type min(num_type a, num_type b)
        {
            JE_math_check_val(a);
            JE_math_check_val(b);
            return a < b ? a : b;
        }

        template<typename num_type>
        static inline num_type min3(num_type a, num_type b, num_type c)
        {
            return min(min(a, b), c);
        }

        template<typename num_type>
        static inline num_type min4(num_type a, num_type b, num_type c, num_type d)
        {
            return min(min(min(a, b), c), d);
        }

        template<typename num_type>
        static inline num_type max(num_type a, num_type b)
        {
            JE_math_check_val(a);
            JE_math_check_val(b);
            return a > b ? a : b;
        }

        template<typename num_type>
        static inline num_type max3(num_type a, num_type b, num_type c)
        {
            return max(max(a, b), c);
        }

        template<typename num_type>
        static inline num_type max4(num_type a, num_type b, num_type c, num_type d)
        {
            return max(max(max(a, b), c), d);
        }

        template<typename num_type>
        static inline num_type clamp(num_type a, num_type min, num_type max)
        {
            JE_math_check_val(a);
            JE_math_check_val(min);
            JE_math_check_val(max);
            JE_assert(min <= max, "Invalid arguments.");
            return max(min(a, max), min);
        }

        template<typename num_type>
        static inline num_type abs(num_type a)
        {
            JE_math_check_val(a);
            return std::abs(a);
        }

        // /////////////////////////

        // Divisions and roundings

        // /////////////////////////

        // Common math functions

        // /////////////////////////

        // Trigonometry

        // /////////////////////////

        // Radians and angles

        // /////////////////////////

        // Interpolation

        /** 
         * Borrowed from UE4.
         * Returns a smooth Hermite interpolation between 0 and 1 for the value a (where a ranges between min and max)
         * Clamped between 0-1.
         */
        static float smoothstep(float a, float min, float max)
        {
            if (a < min)
            {
                return 0.0f;
            }
            else if (a >= max)
            {
                return 1.0f;
            }
            else
            {
                const float interp = (a - min) / (max - min);
                return interp * interp * (3.0f - 2.0f * interp);
            }
        }

        // /////////////////////////

        // Comparisons

        template<typename num_type>
        static inline bool is_within(num_type a, num_type min, num_type max)
        {
            return a >= min && a <= max;
        }

        template<typename num_type>
        static inline bool is_within_exclusive(num_type a, num_type min, num_type max)
        {
            return a > min && a < max;
        }

        static inline bool is_almost_equal(float a, float b);
        static inline bool is_almost_zero(float a);

        static inline bool is_nan(float a)
        {
            return std::isnan(a);
        }

        static inline bool is_inf(float a)
        {
            return std::isinf(a);
        }

        template<typename num_type>
        static inline bool check_val(num_type a)
        {
            // Do not check anything by default.
            return true;
        }

        // ///////////////////////

#if JE_MATH_CHECK_NAN
        static inline float check_val(float a)
        {
            const bool val = (is_nan(a) || is_inf(a)) == false;
            JE_assert(val, "Floating-point value is invalid: [%f]", a);
            return val;
        }
#endif
    };

    template<>
    inline float scalar::abs(float a)
    {
        JE_math_check_val(a);
        return std::fabs(a);
    }

    template<>
    inline bool scalar::is_within(float a, float min, float max)
    {
        JE_math_check_val(a);
        JE_math_check_val(min);
        JE_math_check_val(max);
        return (is_almost_equal(a, min) || a >= min) && (is_almost_equal(a, max) || a <= max);
    }

    template<>
    inline bool scalar::is_within_exclusive(float a, float min, float max)
    {
        JE_math_check_val(a);
        JE_math_check_val(min);
        JE_math_check_val(max);
        return a > min && a < max;
    }

    inline bool scalar::is_almost_equal(float a, float b)
    {
        JE_math_check_val(a);
        JE_math_check_val(b);
        return abs(a - b) <= constants::k_epsilon;
    }

    inline bool scalar::is_almost_zero(float a)
    {
        JE_math_check_val(a);
        return abs(a) <= constants::k_epsilon;
    }
}}
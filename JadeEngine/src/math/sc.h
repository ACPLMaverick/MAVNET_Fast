#pragma once

#include "global.h"
#include "constants.h"
#include "check.h"
#include "interp.h"

#include <cmath>

namespace je { namespace math {

    /**
     * Scalar math.
     */
    class sc : public interp<f32>
    {
    public:

        struct sincos
        {
            f32 sin;
            f32 cos;
        };

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
        static inline num_type clamp(num_type a, num_type a_min, num_type a_max)
        {
            JE_math_check_val(a);
            JE_math_check_val(a_min);
            JE_math_check_val(a_max);
            JE_assert(a_min <= a_max, "Invalid arguments.");
            return max(min(a, a_max), a_min);
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

        static inline bool is_almost_equal(f32 a, f32 b);
        static inline bool is_almost_zero(f32 a);

        // /////////////////////////

        // Common math functions

        template<typename num_type>
        static inline num_type sign(num_type a)
        {
            JE_math_check_val(a);

            if(a >= num_type(0))
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }

        template<typename num_type>
        static inline num_type abs(num_type a)
        {
            return std::abs(a);
        }

        static inline f32 abs(f32 a)
        {
            JE_math_check_val(a);
            return std::fabs(a);
        }

        /**
         * Return a sign(a) * (abs(a) + b).
         */
        template<typename num_type>
        static inline num_type enlarge(num_type a, num_type b)
        {
            JE_math_check_val(a);
            JE_math_check_val(b);
            return sign(a) * (abs(a) + b);
        }

        /**
         * Returns the fractional part of the number without sign.
         */
        static inline f32 fractional(f32 a)
        {
            JE_math_check_val(a);
            const f32 abs_a = fabsf(a);
            const f32 rounded = floor(abs_a);
            return (abs_a - rounded);
        }

        /**
         * Borrowed from UE4.
         * Breaks the given value into an integral and a fractional part.
         * @param a	Floating point value to convert
         * @param out_int_part Floating point value that receives the integral part of the number.
         * @return			The fractional part of the number. This is without sign.
         */
        static inline f32 integral_and_fractional(f32 a, f32& out_int_part)
        {
            JE_math_check_val(a);
            const f32 fractional = modff(a, &out_int_part);
            return abs(fractional);
        }

        static inline f32 pow(f32 a, f32 n)
        {
            JE_math_check_val(a);
            JE_math_check_val(n);
            return std::pow(a, n);
        }

        static inline f32 pow2(f32 a)
        {
            JE_math_check_val(a);
            return a * a;
        }

        static inline f32 pow3(f32 a)
        {
            JE_math_check_val(a);
            return a * a * a;
        }

        /**
         * Returns e^a.
         */
        static inline f32 exp(f32 a)
        {
            JE_math_check_val(a);
            return expf(a);
        }

        /**
         * Returns 2^a.
         */
        static inline f32 exp2(f32 a)
        {
            return pow(2.0f, a);
        }

        static inline f32 ln(f32 a)
        {
            JE_math_check_val(a);
            return logf(a);
        }

        static inline f32 log(f32 base, f32 a)
        {
            return ln(a) / ln(base);
        }

        static inline f32 log2(f32 a)
        {
            static const f32 inv_ln_2 = 1.0f / ln(2.0f);
            return ln(a) * inv_ln_2;
        }

        static inline f32 log10(f32 a)
        {
            static const f32 inv_ln_10 = 1.0f / ln(10.0f);
            return ln(a) * inv_ln_10;
        }

        static inline f32 sqrt(f32 a)
        {
            JE_math_check_val(a);
            return sqrtf(a);
        }

        /** 
         * Borrowed from UE4.
         * Returns the floating-point remainder of a / b
         * Warning: Always returns remainder toward 0, not toward the smaller multiple of b.
         *			So for example modf(2.8f, 2) gives .8f as you would expect, however, modf(-2.8f, 2) gives -.8f, NOT 1.2f 
         * Use Floor instead when snapping positions that can be negative to a grid
         *
         * This is forced to *NOT* inline so that divisions by constant Y does not get optimized in to an inverse sc multiply,
         * which is not consistent with the intent nor with the vectorized version.
         */
        static f32 modf(f32 a, f32 b);

        // /////////////////////////

        // Divisions and roundings

        template<typename num_type>
        static inline num_type divide_ceil(num_type a, num_type b)
        {
            JE_assert(b != 0);
            return (a + b - 1) / b;
        }

        template<typename num_type>
        static inline num_type divide_floor(num_type a, num_type b)
        {
            JE_assert(b != 0);
            return a / b;
        }

        template<typename num_type>
        static inline num_type divide_round(num_type a, num_type b)
        {
            JE_assert(b != 0);
            return (a >= 0) ? (a + b / 2) / b : (a - b / 2 + 1) / b;
        }

        static inline f32 ceil(f32 a)
        {
            JE_math_check_val(a);
            const f32 rounded_down = floor(a);
            if(is_almost_equal(a, rounded_down))
            {
                return a;
            }
            else
            {
                return floor(enlarge(a, 1.0f));
            }
        }

        static inline f32 floor(f32 a)
        {
            JE_math_check_val(a);
            const f32 rounded_down = static_cast<f32>(static_cast<i32>(a));
            if(is_almost_equal(a, rounded_down))
            {
                return a;
            }
            else
            {
                return rounded_down;
            }
        }

        static inline f32 round(f32 a)
        {
            JE_math_check_val(a);
            const f32 rounded_down = floor(a);
            const f32 rounded_down_plus_half = floor(enlarge(a, 0.5f));
            if(is_almost_equal(rounded_down, rounded_down_plus_half))
            {
                return rounded_down;
            }
            else
            {
                return ceil(a);
            }
        }

        // /////////////////////////

        // Trigonometry

        static inline f32 sin(f32 a)
        {
            return sinf(a); 
        }
        
        static inline f32 asin(f32 a)
        {
            return asinf( (a<-1.0f) ? -1.0f : ((a<1.0f) ? a : 1.0f) );
        }

        /**
         * Borrowed from UE4.
         */
        static inline f32 asin_fast(f32 a)
        {
            // Note:  We use FASTASIN_HALF_PI instead of HALF_PI inside of FastASin(), since it was the value that accompanied the minimax coefficients below.
            // It is important to use exactly the same value in all places inside this function to ensure that FastASin(0.0f) == 0.0f.
            // For comparison:
            //		HALF_PI				== 1.57079632679f == 0x3fC90FDB
            //		FASTASIN_HALF_PI	== 1.5707963050f  == 0x3fC90FDA
            static const f32 k_fastasin_pi_2 = 1.5707963050f;

            // Clamp input to [-1,1].
            bool nonnegative = (a >= 0.0f);
            f32 x = abs(a);
            f32 omx = 1.0f - x;
            if (omx < 0.0f)
            {
                omx = 0.0f;
            }
            f32 root = sqrt(omx);
            // 7-degree minimax approximation
            f32 result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + k_fastasin_pi_2;
            result *= root;  // acos(|x|)
            // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
            return (nonnegative ? k_fastasin_pi_2 - result : result - k_fastasin_pi_2);
        }

        static inline f32 sinh(f32 a)
        {
            return sinhf(a);
        }

        static inline f32 cos(f32 a)
        {
            return cosf(a);
        }
        static inline f32 acos(f32 a) 
        {
            return acosf( (a<-1.f) ? -1.f : ((a<1.f) ? a : 1.f) );
        }

        static inline f32 tan(f32 a)
        {
            return tanf(a);
        }

        static inline f32 atan(f32 a)
        {
            return atanf(a);
        }

        /**
         * Borrowed from UE4.
         */
        static inline sincos sin_cos(f32 a)
        {
            // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
            f32 quotient = (constants::k_inv_pi_2) * a;
            if (a >= 0.0f)
            {
                quotient = (f32)((int)(quotient + 0.5f));
            }
            else
            {
                quotient = (f32)((int)(quotient - 0.5f));
            }
            f32 y = a - constants::k_2_pi * quotient;

            // Map y to [-pi/2,pi/2] with sin(y) = sin(a).
            f32 sign;
            if (y > constants::k_pi_2)
            {
                y = constants::k_pi - y;
                sign = -1.0f;
            }
            else if (y < -constants::k_pi_2)
            {
                y = -constants::k_pi - y;
                sign = -1.0f;
            }
            else
            {
                sign = +1.0f;
            }

            const f32 y2 = pow2(y);

            sincos ret_val;

            // 11-degree minimax approximation
            ret_val.sin = ( ( ( ( (-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f ) * y2 + 0.0083333310f ) * y2 - 0.16666667f ) * y2 + 1.0f ) * y;

            // 10-degree minimax approximation
            const f32 p = ( ( ( ( -2.6051615e-07f * y2 + 2.4760495e-05f ) * y2 - 0.0013888378f ) * y2 + 0.041666638f ) * y2 - 0.5f ) * y2 + 1.0f;
            ret_val.cos = sign * p;

            return ret_val;
        }

        /**
         * Borrowed from UE4.
         */
        static f32 atan2(f32 y, f32 x);

        // /////////////////////////

        // Radians and angles

        static f32 rad_to_deg(f32 rad)
        {
            return rad * (180.0f / constants::k_pi);
        }

        static f32 deg_to_rad(f32 deg)
        {
            return deg * (constants::k_pi / 180.0f);
        }

        /**
         * Keep the angle in degrees between -180, 180.
         */
        static f32 unwind_deg(f32 deg)
        {
            while(deg > 180.0f)
            {
                deg -= 360.0f;
            }

            while(deg < -180.0f)
            {
                deg += 360.0f;
            }

            return deg;
        }

        /**
         * Keep the angle in radians between -pi/2, .
         */
        static f32 unwind_rad(f32 rad)
        {
            while(rad > constants::k_pi)
            {
                rad -= constants::k_2_pi;
            }

            while(rad < -constants::k_pi)
            {
                rad += constants::k_2_pi;
            }

            return rad;
        }

        /**
         * Borrowed from UE4.
         * Finds the smallest angle between two directions.
         */
        static f32 find_delta_angle(f32 rad_a, f32 rad_b)
        {
            f32 delta = rad_b - rad_a;

            if(delta > constants::k_pi)
            {
                delta = delta - constants::k_2_pi;
            }
            else if(delta < -constants::k_pi)
            {
                delta = delta + constants::k_2_pi;
            }

            return delta;
        }

        // /////////////////////////
    };

    template<>
    inline bool sc::is_within(f32 a, f32 min, f32 max)
    {
        JE_math_check_val(a);
        JE_math_check_val(min);
        JE_math_check_val(max);
        return (is_almost_equal(a, min) || a >= min) && (is_almost_equal(a, max) || a <= max);
    }

    template<>
    inline bool sc::is_within_exclusive(f32 a, f32 min, f32 max)
    {
        JE_math_check_val(a);
        JE_math_check_val(min);
        JE_math_check_val(max);
        return a > min && a < max;
    }

    inline bool sc::is_almost_equal(f32 a, f32 b)
    {
        JE_math_check_val(a);
        JE_math_check_val(b);
        return abs(a - b) <= constants::k_epsilon;
    }

    inline bool sc::is_almost_zero(f32 a)
    {
        JE_math_check_val(a);
        return abs(a) <= constants::k_epsilon;
    }
}}
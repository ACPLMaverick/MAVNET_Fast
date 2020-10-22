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

        struct sincos
        {
            float sin;
            float cos;
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

        static inline float abs(float a)
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

        static inline float fractional(float a)
        {
            JE_math_check_val(a);
            const float abs_a = fabsf(a);
            const float rounded = floor(abs_a);
            return sign(a) * (abs_a - rounded);
        }

        /**
         * Borrowed from UE4.
         * Breaks the given value into an integral and a fractional part.
         * @param a	Floating point value to convert
         * @param out_int_part Floating point value that receives the integral part of the number.
         * @return			The fractional part of the number.
         */
        static inline float break_integral_and_fractional(float a, float& out_int_part)
        {
            JE_math_check_val(a);
            return modff(a, &out_int_part);
        }

        static inline float pow(float a, float n)
        {
            JE_math_check_val(a);
            JE_math_check_val(n);
            return std::pow(a, n);
        }

        static inline float pow2(float a)
        {
            JE_math_check_val(a);
            return a * a;
        }

        static inline float pow3(float a)
        {
            JE_math_check_val(a);
            return a * a * a;
        }

        /**
         * Returns e^a.
         */
        static inline float exp(float a)
        {
            JE_math_check_val(a);
            return expf(a);
        }

        /**
         * Returns 2^a.
         */
        static inline float exp2(float a)
        {
            return pow(2.0f, a);
        }

        static inline float ln(float a)
        {
            JE_math_check_val(a);
            return logf(a);
        }

        static inline float log(float base, float a)
        {
            return ln(a) / ln(base);
        }

        static inline float log2(float a)
        {
            static const float inv_ln_2 = 1.0f / ln(2.0f);
            return ln(a) * inv_ln_2;
        }

        static inline float log10(float a)
        {
            static const float inv_ln_10 = 1.0f / ln(10.0f);
            return ln(a) * inv_ln_10;
        }

        static inline float sqrt(float a)
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
         * This is forced to *NOT* inline so that divisions by constant Y does not get optimized in to an inverse scalar multiply,
         * which is not consistent with the intent nor with the vectorized version.
         */
        static float modf(float a, float b);

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

        static inline float ceil(float a)
        {
            JE_math_check_val(a);
            const float rounded_down = floor(a);
            if(is_almost_equal(a, rounded_down))
            {
                return a;
            }
            else
            {
                return floor(enlarge(a, 1.0f));
            }
        }

        static inline float floor(float a)
        {
            JE_math_check_val(a);
            const float rounded_down = static_cast<float>(static_cast<int32_t>(a));
            if(is_almost_equal(a, rounded_down))
            {
                return a;
            }
            else
            {
                return rounded_down;
            }
        }

        static inline float round(float a)
        {
            JE_math_check_val(a);
            const float rounded_down = floor(a);
            const float rounded_down_plus_half = floor(enlarge(a, 0.5f));
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

        static inline float sin(float a)
        {
            return sinf(a); 
        }
        
        static inline float asin(float a)
        {
            return asinf( (a<-1.f) ? -1.f : ((a<1.f) ? a : 1.f) );
        }

        /**
         * Borrowed from UE4.
         */
        static inline float asin_fast(float a)
        {
            // Note:  We use FASTASIN_HALF_PI instead of HALF_PI inside of FastASin(), since it was the value that accompanied the minimax coefficients below.
            // It is important to use exactly the same value in all places inside this function to ensure that FastASin(0.0f) == 0.0f.
            // For comparison:
            //		HALF_PI				== 1.57079632679f == 0x3fC90FDB
            //		FASTASIN_HALF_PI	== 1.5707963050f  == 0x3fC90FDA
            static const float k_fastasin_pi_2 = 1.5707963050f;

            // Clamp input to [-1,1].
            bool nonnegative = (a >= 0.0f);
            float x = abs(a);
            float omx = 1.0f - x;
            if (omx < 0.0f)
            {
                omx = 0.0f;
            }
            float root = sqrt(omx);
            // 7-degree minimax approximation
            float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + k_fastasin_pi_2;
            result *= root;  // acos(|x|)
            // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
            return (nonnegative ? k_fastasin_pi_2 - result : result - k_fastasin_pi_2);
        }

        static inline float sinh(float a)
        {
            return sinhf(a);
        }

        static inline float cos(float a)
        {
            return cosf(a);
        }
        static inline float acos(float a) 
        {
            return acosf( (a<-1.f) ? -1.f : ((a<1.f) ? a : 1.f) );
        }

        static inline float tan(float a)
        {
            return tanf(a);
        }

        static inline float atan(float a)
        {
            return atanf(a);
        }

        /**
         * Borrowed from UE4.
         */
        static inline sincos sin_cos(float a)
        {
            // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
            float quotient = (constants::k_inv_pi_2) * a;
            if (a >= 0.0f)
            {
                quotient = (float)((int)(quotient + 0.5f));
            }
            else
            {
                quotient = (float)((int)(quotient - 0.5f));
            }
            float y = a - constants::k_2_pi * quotient;

            // Map y to [-pi/2,pi/2] with sin(y) = sin(a).
            float sign;
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

            const float y2 = pow2(y);

            sincos ret_val;

            // 11-degree minimax approximation
            ret_val.sin = ( ( ( ( (-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f ) * y2 + 0.0083333310f ) * y2 - 0.16666667f ) * y2 + 1.0f ) * y;

            // 10-degree minimax approximation
            const float p = ( ( ( ( -2.6051615e-07f * y2 + 2.4760495e-05f ) * y2 - 0.0013888378f ) * y2 + 0.041666638f ) * y2 - 0.5f ) * y2 + 1.0f;
            ret_val.cos = sign * p;

            return ret_val;
        }

        /**
         * Borrowed from UE4.
         */
        static float atan2(float y, float x);

        // /////////////////////////

        // Radians and angles

        static float rad_to_deg(float rad)
        {
            return rad * (180.0f / constants::k_pi);
        }

        static float deg_to_rad(float deg)
        {
            return deg * (constants::k_pi / 180.0f);
        }

        static float unwind_deg(float deg)
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

        static float unwind_rad(float rad)
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
        static float find_delta_angle(float rad_a, float rad_b)
        {
            float delta = rad_b - rad_a;

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
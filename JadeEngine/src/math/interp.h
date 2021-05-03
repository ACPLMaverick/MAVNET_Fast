#pragma once

#include "global.h"
#include "constants.h"
#include "check.h"

#include <cmath>

namespace je { namespace math {

    /**
     * Most of these are borrowed from UE4.
     */
    template<typename interp_type>
    class interp
    {
    public:

        /** Classic linear interpolation. */
        static inline interp_type lerp(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(a);
            JE_math_check_val(b);
            JE_math_check_val(alpha);
            return a * (1.0f - alpha) + b * alpha;
        }

        /** Performs a 2D linear interpolation between four values values, frac_x, frac_y ranges from 0-1 */
        static inline interp_type lerp_2D(
            const interp_type& p_00,
            const interp_type& p_10,
            const interp_type& p_01,
            const interp_type& p_11,
            f32 frac_x, f32 frac_y)
        {
            return lerp
                (
                    lerp(p_00, p_10, frac_x),
                    lerp(p_01, p_11, frac_x),
                    frac_y
                );
        }

        /**
         * Performs a cubic interpolation
         *
         * @param  p - end points
         * @param  t - tangent directions at end points
         * @param  alpha - distance along spline
         *
         * @return  Interpolated value
         */
        static inline interp_type interp_cubic(const interp_type& p_0, const interp_type& t_0, const interp_type& p_1, const interp_type& t_1, f32 alpha)
        {
            JE_math_check_val(p_0);
            JE_math_check_val(p_1);
            JE_math_check_val(t_0);
            JE_math_check_val(t_1);
            JE_math_check_val(alpha);

            const f32 a2 = alpha  * alpha;
            const f32 a3 = a2 * alpha;

            return (((2.0f * a3) - (3.0f * a2) + 1) * p_0) + ((a3 - (2.0f * a2) + alpha) * t_0) + ((a3 - a2) * t_1) + (((-2.0f * a3)+(3.0f * a2)) * p_1);
        }

        /**
         * Performs a first derivative cubic interpolation
         *
         * @param  p - end points
         * @param  t - tangent directions at end points
         * @param  alpha - distance along spline
         *
         * @return  Interpolated value
         */
        static inline interp_type interp_cubic_derivative(const interp_type& p_0, const interp_type& t_0, const interp_type& p_1, const interp_type& t_1, f32 alpha)
        {
            JE_math_check_val(p_0);
            JE_math_check_val(p_1);
            JE_math_check_val(t_0);
            JE_math_check_val(t_1);
            JE_math_check_val(alpha);

            interp_type a = 6.0f * p_0 + 3.0f * t_0 + 3.0f * t_1 - 6.0f * p_1;
            interp_type b = -6.0f * p_0 - 4.0f * t_0 - 2.0f * t_1 + 6.0f * p_1;
            interp_type c = t_0;

            const f32 a2 = alpha * alpha;

            return (a * a2) + (b * alpha) + c;
        }

        /**
         * Performs a second derivative cubic interpolation
         *
         * @param  p - end points
         * @param  t - tangent directions at end points
         * @param  alpha - distance along spline
         *
         * @return  Interpolated value
         */
        static inline interp_type interp_cubic_second_derivative(const interp_type& p_0, const interp_type& t_0, const interp_type& p_1, const interp_type& t_1, f32 alpha)
        {
            JE_math_check_val(p_0);
            JE_math_check_val(p_1);
            JE_math_check_val(t_0);
            JE_math_check_val(t_1);
            JE_math_check_val(alpha);

            interp_type a = 12.0f * p_0 + 6.0f * t_0 + 6.0f * t_1 - 12.0f * p_1;
            interp_type b = -6.0f * p_0 - 4.0f * t_0 - 2.0f * t_1 + 6.0f * p_1;

            return (a * alpha) + b;
        }

        /** Interpolate between a and b, applying an ease in function.  exp controls the degree of the curve. */
        static inline interp_type interp_ease_in(const interp_type& a, const interp_type& b, f32 alpha, f32 exp)
        {
            JE_math_check_val(alpha);
            JE_math_check_val(exp);
            const f32 modified_alpha = powf(alpha, exp);
            return lerp(a, b, modified_alpha);
        }

        /** Interpolate between a and b, applying an ease out function.  exp controls the degree of the curve. */
        static inline interp_type interp_ease_out(const interp_type& a, const interp_type& b, f32 alpha, f32 exp)
        {
            JE_math_check_val(alpha);
            JE_math_check_val(exp);
            const f32 modified_alpha = 1.0f - powf(1.0f - alpha, exp);
            return lerp(a, b, modified_alpha);
        }

        /** Interpolate between a and b, applying an ease in/out function.  exp controls the degree of the curve. */
        static inline interp_type interp_ease_in_out(const interp_type& a, const interp_type& b, f32 alpha, f32 exp)
        {
            JE_math_check_val(alpha);
            JE_math_check_val(exp);
            return lerp(a, b, (alpha < 0.5f) ?
                interp_ease_in(0.0f, 1.0f, alpha * 2.0f, exp) * 0.5f :
                interp_ease_out(0.0f, 1.0f, alpha * 2.0f - 1.f, exp) * 0.5f + 0.5f);
        }

        /** Interpolation between a and b, applying a step function. */
        static inline interp_type interp_step(const interp_type& a, const interp_type& b, f32 alpha, i32 num_steps)
        {
            JE_math_check_val(alpha);

            if (num_steps <= 1 || alpha <= 0.0f)
            {
                return a;
            }
            else if (alpha >= 1.0f)
            {
                return b;
            }

            const f32 steps_as_float = static_cast<f32>(num_steps);
            const f32 num_intervals = steps_as_float - 1.f;
            const f32 modified_alpha = static_cast<f32>(static_cast<i32>((alpha * steps_as_float))) / num_intervals;
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying a sinusoidal in function. */
        static inline interp_type interp_sin_in(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            const f32 modified_alpha = -1.0f * cosf(alpha * constants::k_pi_2) + 1.f;
            return lerp(a, b, modified_alpha);
        }
        
        /** Interpolation between a and b, applying a sinusoidal out function. */
        static inline interp_type interp_sin_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            const f32 modified_alpha = sinf(alpha * constants::k_pi_2);
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying a sinusoidal in/out function. */
        static inline interp_type interp_sin_in_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            return lerp(a, b, (alpha < 0.5f) ?
                interp_sin_in(0.0f, 1.0f, alpha * 2.0f) * 0.5f :
                interp_sin_out(0.0f, 1.0f, alpha * 2.0f - 1.0f) * 0.5f + 0.5f);
        }

        /** Interpolation between a and b, applying an exponential in function. */
        static inline interp_type interp_exp_in(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            const f32 modified_alpha = (alpha == 0.0f) ? 0.0f : powf(2.0f, 10.f * (alpha - 1.0f));
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying an exponential out function. */
        static inline interp_type interp_exp_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            const f32 modified_alpha = (alpha == 1.f) ? 1.f : -powf(2.0f, -10.0f * alpha) + 1.0f;
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying an exponential in/out function. */
        static inline interp_type interp_exp_in_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            return lerp(a, b, (alpha < 0.5f) ?
                interp_exp_in(0.0f, 1.0f, alpha * 2.0f) * 0.5f :
                interp_exp_out(0.0f, 1.0f, alpha * 2.0f - 1.0f) * 0.5f + 0.5f);
        }

        /** Interpolation between a and b, applying a circular in function. */
        static inline interp_type interp_circular_in(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            const f32 modified_alpha = -1.0f * (sqrtf(1.0f - alpha * alpha) - 1.0f);
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying a circular out function. */
        static inline interp_type interp_circular_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            alpha -= 1.0f;
            const f32 modified_alpha = sqrtf(1.0f - alpha  * alpha);
            return lerp(a, b, modified_alpha);
        }

        /** Interpolation between a and b, applying a circular in/out function. */
        static inline interp_type interp_circular_in_out(const interp_type& a, const interp_type& b, f32 alpha)
        {
            JE_math_check_val(alpha);
            return lerp(a, b, (alpha < 0.5f) ?
                interp_circular_in(0.0f, 1.0f, alpha * 2.0f) * 0.5f :
                interp_circular_out(0.0f, 1.0f, alpha * 2.0f - 1.0f) * 0.5f + 0.5f);
        }

        /** 
         * Returns a smooth Hermite interpolation between 0 and 1 for the value a (where a ranges between min and max)
         * Clamped between 0-1.
         */
        static inline interp_type smoothstep(const interp_type& a, const interp_type& min, const interp_type& max)
        {
            JE_math_check_val(a);
            JE_math_check_val(min);
            JE_math_check_val(max);
            
            if (a < min)
            {
                return interp_type(0.0f);
            }
            else if (a >= max)
            {
                return interp_type(1.0f);
            }
            else
            {
                const interp_type interp = (a - min) / (max - min);
                return interp * interp * (interp_type(3.0f) - interp_type(2.0f) * interp);
            }
        }
    };

}}
#include "sc.h"

namespace je { namespace math {

    float sc::modf(float a, float b)
    {
        JE_math_check_val(a);
        JE_math_check_val(b);

        const float abs_y = abs(b);
        if (abs_y <= 1.e-8f)
        {
            JE_fail("Invalid argument for modf.");
            return 0.f;
        }
        const float div = (a / b);
        // All floats where abs(f) >= 2^23 (8388608) are whole numbers so do not need truncation, and avoid overflow in TruncToFloat as they get even larger.
        const float quotient = abs(div) < constants::k_float_non_fractional ? floor(div) : div;
        float int_portion = b * quotient;

        // Rounding and imprecision could cause int_portion to exceed a and cause the result to be outside the expected range.
        // For example Fmod(55.8, 9.3) would result in a very small negative value!
        if (fabsf(int_portion) > fabsf(a))
        {
            int_portion = a;
        }

        const float result = a - int_portion;
        // Clamp to [-abs_y, abs_y] because of possible failures for very large numbers (>1e10) due to precision loss.
        // We could instead fall back to stock fmodf() for large values, however this would diverge from the SIMD VectorMod() which has no similar fallback with reasonable performance.
        return clamp(result, -abs_y, abs_y);
    }

    float sc::atan2(float y, float x)
    {
        // return atan2f(y,x);
        // atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
        // We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
        // On PC this has been measured to be 2x faster than the std C version.

        const float absX = abs(x);
        const float absY = abs(y);
        const bool yAbsBigger = (absY > absX);
        float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
        float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)
        
        if (t0 == 0.f)
            return 0.f;

        float t3 = t1 / t0;
        float t4 = t3 * t3;

        static const float c[7] = {
            +7.2128853633444123e-03f,
            -3.5059680836411644e-02f,
            +8.1675882859940430e-02f,
            -1.3374657325451267e-01f,
            +1.9856563505717162e-01f,
            -3.3324998579202170e-01f,
            +1.0f
        };

        t0 = c[0];
        t0 = t0 * t4 + c[1];
        t0 = t0 * t4 + c[2];
        t0 = t0 * t4 + c[3];
        t0 = t0 * t4 + c[4];
        t0 = t0 * t4 + c[5];
        t0 = t0 * t4 + c[6];
        t3 = t0 * t3;

        t3 = yAbsBigger ? (0.5f * constants::k_pi) - t3 : t3;
        t3 = (x < 0.0f) ? constants::k_pi - t3 : t3;
        t3 = (y < 0.0f) ? -t3 : t3;

        return t3;
    }

}}
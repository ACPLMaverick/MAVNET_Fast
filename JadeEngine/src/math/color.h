#pragma once

#include "global.h"
#include "math/vec.h"
#include "math/sc.h"

namespace je { namespace math {

    class color
    {
    public:

        static const size k_num_components = 4;

        color()
            : m_packed(0)
        {
        }

        color(u8 a_red, u8 a_green, u8 a_blue, u8 a_alpha = 0)
        {
            red() = a_red;
            green() = a_green;
            blue() = a_blue;
            alpha() = a_alpha;
        }

        color(float a_red, float a_green, float a_blue, float a_alpha = 0.0f)
        {
            set_red_float(a_red);
            set_green_float(a_green);
            set_blue_float(a_blue);
            set_alpha_float(a_alpha);
        }

        u32 get_packed()
        {
            return m_packed;
        }

        u8 get_red() const
        {
            return const_cast<color*>(this)->red();
        }

        u8 get_green() const
        {
            return const_cast<color*>(this)->green();
        }

        u8 get_blue() const
        {
            return const_cast<color*>(this)->blue();
        }

        u8 get_alpha() const
        {
            return const_cast<color*>(this)->alpha();
        }

        u8& red()
        {
            return m_data[2];
        }

        u8& green()
        {
            return m_data[1];
        }

        u8& blue()
        {
            return m_data[0];
        }

        u8& alpha()
        {
            return m_data[3];
        }

        float get_red_float() const
        {
            return to_float(get_red());
        }

        float get_green_float() const
        {
            return to_float(get_green());
        }

        float get_blue_float() const
        {
            return to_float(get_blue());
        }

        float get_alpha_float() const
        {
            return to_float(get_alpha());
        }

        void set_red_float(float a_component)
        {
            red() = from_float(a_component);
        }

        void set_green_float(float a_component)
        {
            green() = from_float(a_component);
        }

        void set_blue_float(float a_component)
        {
            blue() = from_float(a_component);
        }

        void set_alpha_float(float a_component)
        {
            alpha() = from_float(a_component);
        }

        bool is_with_alpha() const
        {
            return get_alpha() > 0;
        }

        vec4 to_float_vec4() const
        {
            return vec4(get_red_float(), get_green_float(), get_blue_float(), get_alpha_float());
        }

        vec3 to_float_vec3() const
        {
            JE_assert(is_with_alpha() == false, "Truncating alpha.");
            return vec3(get_red_float(), get_green_float(), get_blue_float());
        }
        
    private:

        static float to_float(u8 a_component)
        {
            return sc::clamp(static_cast<float>(a_component) / 255.0f, 0.0f, 1.0f);
        }

        static u8 from_float(float a_component)
        {
            return static_cast<u8>(sc::clamp(a_component, 0.0f, 1.0f) * 255.0f);
        }

        union
        {
            u8 m_data[k_num_components];
            u32 m_packed;
        };
    };

}}
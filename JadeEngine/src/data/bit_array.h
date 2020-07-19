#pragma once

#include "array.h"

namespace je { namespace data {

    template <size_t num_bits>
    class bit_array : public array<uint8_t, (num_bits / 8) + (num_bits % 8 == 0 ? 0 : 1)>
    {
    public:

        static const size_t k_num_bits = num_bits;
    };

}}
#include "GlobalIncludes.h"

bool IsBitWidthEqualOrLesserThan(uint64_t val, uint8_t bitWidth)
{
	JE_Assert(bitWidth <= 64);
	if (bitWidth == 0)
	{
		return val == 0;
	}

	uint64_t checkValue = 1;
	for (int i = 0; i < bitWidth - 1; ++i)
	{
		checkValue |= (checkValue << 1);
	}

	checkValue = ~checkValue;

	return val & checkValue;
}
#include "Random.h"

#include <random>

namespace Util
{
	Random::Random()
	{
	}

	Random::~Random()
	{

	}

	uint32_t Random::Get32(uint32_t minimum /*= 0*/, uint32_t maximum /*= std::numeric_limits<uint32_t>::max()*/)
	{
		JE_Assert(minimum <= maximum);
		static std::subtract_with_carry_engine<uint32_t, _wordSize, _shortLag, _longLag> generator(GetSeed());
		return (generator() % (maximum - minimum + 1)) + minimum;
	}

	uint64_t Random::Get64(uint64_t minimum /*= 0*/, uint64_t maximum /*= std::numeric_limits<uint64_t>::max()*/)
	{
		JE_Assert(minimum <= maximum);
		static std::subtract_with_carry_engine<uint64_t, _wordSize, _shortLag, _longLag> generator(GetSeed());
		return (generator() % (maximum - minimum + 1)) + minimum;
	}

	float Random::GetFloat(float minimum /*= std::numeric_limits<float>::min()*/, float maximum /*= std::numeric_limits<float>::max()*/)
	{
		JE_Assert(minimum <= maximum);
		static std::subtract_with_carry_engine<uint32_t, _wordSize, _shortLag, _longLag> generator(GetSeed());
		return (fmodf(static_cast<float>(generator()), (maximum - minimum + 1.0f))) + minimum;
	}

	uint64_t Random::GetSeed()
	{
		return std::chrono::system_clock::now().time_since_epoch().count();
	}
}
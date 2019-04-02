#pragma once

namespace Util
{
	class Random
	{
	public:
		Random();
		~Random();

		// All retured values are inclusive.
		uint32_t Get32(uint32_t minimum = 0, uint32_t maximum = std::numeric_limits<uint32_t>::max());
		uint64_t Get64(uint64_t minimum = 0, uint64_t maximum = std::numeric_limits<uint64_t>::max());
		float GetFloat(float minimum = std::numeric_limits<float>::min(), float maximum = std::numeric_limits<float>::max());

	private:

		uint64_t GetSeed();
		
		static constexpr uint32_t _wordSize = 24;
		static constexpr uint32_t _shortLag = 10;
		static constexpr uint32_t _longLag = 24;
	};
}
#pragma once

#include "Resource.h"
#include "UboCommon.h"

namespace Rendering
{
	class UniformBuffer : public Resource
	{
	public:

		struct Options
		{
			size_t DataSize;

			Options()
				: DataSize(0)
			{
			}
		};

	public:

		UniformBuffer();
		~UniformBuffer();

		void Initialize(const Options* options);
		void Cleanup();

		void UpdateWithData(uint8_t* data, size_t dataSize, size_t offset = 0);

		JE_Inline const Options* GetOptions() const { return &_options; }
		JE_Inline VkBuffer GetBuffer() const { return _buffer; }

	protected:

		Options _options;

		VkDeviceMemory _deviceMemory;
		VkBuffer _buffer;
	};
}
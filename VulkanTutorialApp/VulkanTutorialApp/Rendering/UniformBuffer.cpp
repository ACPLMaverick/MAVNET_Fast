#include "UniformBuffer.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	UniformBuffer::UniformBuffer() 
		: Resource()
		, _buffer(VK_NULL_HANDLE)
		, _deviceMemory(VK_NULL_HANDLE)
	{
		_type = ResourceCommon::Type::UniformBuffer;
	}


	UniformBuffer::~UniformBuffer()
	{
	}

	void UniformBuffer::Initialize(const Options * options)
	{
		_options = *options;

		JE_GetRenderer()->CreateBuffer(_options.DataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _buffer, _deviceMemory);
	}

	void UniformBuffer::Cleanup()
	{
		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), _buffer, JE_GetRenderer()->GetAllocatorPtr());
		_buffer = VK_NULL_HANDLE;
		vkFreeMemory(JE_GetRenderer()->GetDevice(), _deviceMemory, JE_GetRenderer()->GetAllocatorPtr());
		_deviceMemory = VK_NULL_HANDLE;
	}

	void UniformBuffer::UpdateWithData(uint8_t * data, size_t dataSize, size_t offset)
	{
		JE_Assert(dataSize <= _options.DataSize);
		JE_Assert(offset <= _options.DataSize - dataSize);

		JE_GetRenderer()->CopyBuffer_CPU_GPU(data + offset, _deviceMemory, dataSize);
	}
}
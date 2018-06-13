#include "Helper.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	void Helper::Initialize()
	{
		InitializeVoidVertexBuffer();
	}

	void Helper::Cleanup()
	{
		CleanupVoidVertexBuffer();
	}

	VkBuffer Helper::GetVoidVertexBuffer(uint32_t requestedMinSizeBytes)
	{
		if (requestedMinSizeBytes > GetVoidVertexBufferSizeBytes())
		{
			_voidVertexBufferCurrentElementCount = requestedMinSizeBytes / _voidVertexBufferElementSize;

			CleanupVoidVertexBuffer();
			InitializeVoidVertexBuffer();
		}

		return _voidVertexBuffer;
	}

	void Helper::InitializeVoidVertexBuffer()
	{
		VkDeviceSize bufferSize = GetVoidVertexBufferSizeBytes();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// Fill with ones.
		float* arr = static_cast<float*>(malloc(bufferSize));
		for (size_t i = 0; i < _voidVertexBufferCurrentElementCount; ++i)
			arr[i] = 1.0f;

		JE_GetRenderer()->CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(arr), stagingBufferMemory, static_cast<size_t>(bufferSize));

		free(arr);

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _voidVertexBuffer, _voidVertexBufferMemory);

		JE_GetRenderer()->CopyBuffer_GPU_GPU(stagingBuffer, _voidVertexBuffer, bufferSize);

		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), stagingBuffer, JE_GetRenderer()->GetAllocatorPtr());
		vkFreeMemory(JE_GetRenderer()->GetDevice(), stagingBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
	}

	void Helper::CleanupVoidVertexBuffer()
	{
		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), _voidVertexBuffer, JE_GetRenderer()->GetAllocatorPtr());
		vkFreeMemory(JE_GetRenderer()->GetDevice(), _voidVertexBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
	}
}
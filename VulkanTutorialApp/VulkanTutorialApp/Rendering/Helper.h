#pragma once

namespace Rendering
{
	class Helper : public Util::Singleton<Helper>
	{
		friend class Util::Singleton<Helper>;

	public:

		void Initialize();
		void Cleanup();

		VkBuffer GetVoidVertexBuffer(uint32_t requestedMinSizeBytes = 0);

	private:
		Helper() {}
		~Helper() {}

		void InitializeVoidVertexBuffer();
		void CleanupVoidVertexBuffer();
		JE_Inline uint32_t GetVoidVertexBufferSizeBytes() const { return _voidVertexBufferElementSize * _voidVertexBufferCurrentElementCount; }

	private:

		static const uint32_t _voidVertexBufferElementSize = sizeof(float);

		uint32_t _voidVertexBufferCurrentElementCount = static_cast<uint32_t>(static_cast<uint16_t>(-1));

		VkDeviceMemory _voidVertexBufferMemory;
		VkBuffer _voidVertexBuffer;

	};
}
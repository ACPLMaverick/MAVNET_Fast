#pragma once

#include "Shader.h"
#include "RenderState.h"
#include "DescriptorCommon.h"

namespace Rendering
{
	class Pipeline
	{
	public:

		static const uint32_t KEY_PREFERRED_SIZE_BYTES = 32;	
		// Padding for cache line, current size is 52 bytes.

#pragma pack(push, 1)
		struct Key
		{
			RenderState::Key KeyRenderState;
			Shader::Key KeyShader;
			// DescriptorLayout does not take part in the key because although there can be many shaders with the same layout, there cannot be many layouts with the same shader.

			bool operator==(const Key& other) const
			{
				JE_AssertStatic(sizeof(Key) <= KEY_PREFERRED_SIZE_BYTES);
				return
					KeyRenderState == other.KeyRenderState
					&& KeyShader == other.KeyShader;
			}
		};
#pragma pack(pop)

	public:

		Pipeline() { }
		~Pipeline() { }

		void Initialize(const RenderState::Info* renderStateInfo, const Shader* shader);
		void Cleanup();

	private:

		RenderState _renderState = {};
		const Shader* _shader = nullptr;

	};
}

namespace std
{
	template<> struct hash<::Rendering::Pipeline::Key>
	{
		size_t operator()(const ::Rendering::Pipeline::Key& key) const
		{
			JE_AssertStatic(sizeof(::Rendering::Pipeline::Key) % sizeof(size_t) == 0);
			size_t finalRet = 0;
			const size_t hashNum = ::Rendering::Pipeline::KEY_PREFERRED_SIZE_BYTES / sizeof(size_t);
			const size_t hashes[hashNum] = {};

			const size_t* dataPtr = reinterpret_cast<const size_t*>(&key);
			for (size_t i = 0; i < hashNum; ++i, ++dataPtr)
			{
				finalRet = (finalRet ^ std::hash<size_t>{}(*dataPtr) << i) >> 1;
			}

			return finalRet;
		}
	};
}
#pragma once

#include "Rendering/resource/Shader.h"
#include "RenderState.h"
#include "Rendering/descriptor/DescriptorCommon.h"
#include "Rendering/renderPass/RenderPassCommon.h"

namespace Rendering
{
	class DescriptorSet;
	class VertexDeclaration;
	class RenderPass;
	struct InitDataPipeline;

	class Pipeline
	{
	public:

		static const uint32_t KEY_PREFERRED_SIZE_BYTES = 32;	
		// Padding for cache line, current size is 32 bytes.

		enum Type
		{
			Graphics,
			Compute
		};

#pragma pack(push, 1)
		struct alignas(KEY_PREFERRED_SIZE_BYTES) Key
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

		struct Info
		{
			const RenderState::Info* RenderStateInfo;
			const Shader* MyShader;
			const DescriptorCommon::LayoutData* DescriptorLayoutData;
			const VertexDeclaration* MyVertexDeclaration;
			Type MyType;
			RenderPassCommon::Id MyPass;
		};

	public:

		Pipeline() { }
		~Pipeline() { JE_Assert(_associatedShader == nullptr); }

		void Initialize
		(
			const Info* initData
		);
		void Cleanup();

		static void CreateKey(const Info* info, Key* outKey);

		JE_Inline const Shader* GetAssociatedShader() const { return _associatedShader; }
		JE_Inline Type GetType() const { return _type; }

	private:

		RenderState _renderState = {};
		const Shader* _associatedShader = nullptr;
		const DescriptorCommon::LayoutData* _associatedDescriptorLayoutData;
		const VertexDeclaration* _associatedVertexDeclaration;
		const RenderPass* _associatedRenderPass;
		Type _type;

		VkPipelineLayout _associatedPipelineLayout = VK_NULL_HANDLE;	// TODO: Can there be many pipelines with the same pipeline layout? I think yes.
		VkPipeline _pipeline = VK_NULL_HANDLE;
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
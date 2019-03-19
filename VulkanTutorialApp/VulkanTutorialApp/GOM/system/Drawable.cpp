#include "Drawable.h"

#include "Rendering/resource/Mesh.h"
#include "Rendering/resource/Material.h"

#include "Core/HelloTriangle.h"
#include "Rendering/Helper.h"

namespace GOM
{
	void DrawableBehaviour::Update()
	{
		for (SystemObject* objAbstract : _objectsAll)
		{
			DrawableObject* obj = ObjectCast(objAbstract);

			obj->PropMaterial->Update();
		}
	}

	void DrawableBehaviour::Draw()
	{
		using namespace Rendering;

		VkCommandBuffer cmd = JE_GetRenderer()->GetCmd();
		uint32_t currentSubpass = 0;
		RenderPassCommon::Id currentRenderPass = JE_GetRenderer()->GetActiveRenderPass(&currentSubpass);
		// Cmd must be in a recording state!

		std::vector<VkCommandBuffer> secondaries;
		for (SystemObject* objAbstract : _objectsAll)
		{
			DrawableObject* obj = ObjectCast(objAbstract);
			JE_Assert(obj);

			secondaries.push_back(obj->_secondaryCommandBuffer.GetVkCommandBuffer(currentRenderPass, currentSubpass));
		}

		if (secondaries.size() > 0)
		{
			JE_Assert(secondaries.size() < std::numeric_limits<uint32_t>::max());
			vkCmdExecuteCommands(cmd, (uint32_t)secondaries.size(), secondaries.data());
		}
	}

	SystemObject * DrawableBehaviour::ConstructObject_Internal()
	{
		return new DrawableObject();
	}

	void DrawableBehaviour::InitializeObject_Internal(SystemObject * objAbstract)
	{
		DrawableObject* obj = ObjectCast(objAbstract);
		
		AdjustBuffersForVertexDeclaration(obj);
		CreateSecondaryCommandBuffer(obj);
	}

	void DrawableBehaviour::CleanupObject_Internal(SystemObject * objAbstract)
	{
		DrawableObject* obj = ObjectCast(objAbstract);
		
		obj->_adjVertexBufferArray.clear();
		obj->_adjOffsetArray.clear();

		obj->_secondaryCommandBuffer.Cleanup();

		obj->PropMaterial = nullptr;
		obj->PropMesh = nullptr;
	}

	void DrawableBehaviour::CloneObject_Internal(SystemObject * destinationAbstract, const SystemObject * sourceAbstract)
	{
		DrawableObject* destination = ObjectCast(destinationAbstract);
		const DrawableObject* source = ObjectCast(sourceAbstract);

		destination->PropMesh = source->PropMesh;
		destination->PropMaterial = source->PropMaterial;

		destination->_adjVertexBufferArray = source->_adjVertexBufferArray;
		destination->_adjOffsetArray = source->_adjOffsetArray;
		destination->_secondaryCommandBuffer = source->_secondaryCommandBuffer;
	}

	void DrawableBehaviour::AdjustBuffersForVertexDeclaration(DrawableObject * obj)
	{
		using namespace Rendering;

		const VertexDeclaration* declaration = obj->PropMaterial->GetVertexDeclaration();
		const std::vector<VkBuffer>* vertexBufferArray = obj->PropMesh->GetVertexBuffers();
		const std::vector<VkDeviceSize>* offsetArray = obj->PropMesh->GetVertexBufferOffsets();
		const uint32_t bufferNum = obj->PropMesh->GetVertexBufferCount();
		const Mesh::Info* meshInfo = obj->PropMesh->GetInfo();

		JE_Assert(declaration);
		JE_Assert(vertexBufferArray);
		JE_Assert(offsetArray);

		const size_t componentCount = declaration->GetComponents()->size();
		obj->_adjVertexBufferArray.clear();
		obj->_adjVertexBufferArray.resize(componentCount);
		obj->_adjOffsetArray.clear();
		obj->_adjOffsetArray.resize(componentCount);

		for (size_t i = 0; i < componentCount; ++i)
		{
			VertexDeclaration::ComponentType thisType = (*declaration->GetComponents())[i];

			// Find this type in our vertex arrays.
			size_t foundIndex = -1;
			for (size_t j = 0; j < meshInfo->VertexArrays.size(); ++j)
			{
				if (meshInfo->VertexArrays[j].Type == thisType)
				{
					foundIndex = j;
					break;
				}
			}

			if (foundIndex != -1)
			{
				// Found this amongst vertex arrays. Assuming indices in vertex arrays correspond to these in vertex buffer array.
				// Simply place this buffer in this position.

				obj->_adjVertexBufferArray[i] = (*vertexBufferArray)[foundIndex];
				obj->_adjOffsetArray[i] = (*offsetArray)[foundIndex];
			}
			else
			{
				// This component type is not present among the vertex arrays of this mesh.
				// Place dummy vertex buffer from the helper.

				uint32_t neededSizeBytes = VertexDeclaration::GetComponentSize(thisType) * meshInfo->VertexCount;

				obj->_adjVertexBufferArray[i] = Helper::GetInstance()->GetVoidVertexBuffer(neededSizeBytes);
				obj->_adjOffsetArray[i] = 0;
			}
		}
	}

	void DrawableBehaviour::CreateSecondaryCommandBuffer(DrawableObject * obj)
	{
		using namespace Rendering;

		SecondaryCommandBuffer::Info info;
		info.Context = (SecondaryCommandBuffer::RecordContext)obj;
		info.RecordFunc = &DrawableBehaviour::RecordFunc;

		// Here add all passes that are drawable object can be used in. TODO: Can be retrieved from material.
		SecondaryCommandBuffer::CompatibleRenderPassData passData;
		passData.Id = RenderPassCommon::Id::Tutorial;
		info.CompatibleRenderPasses.push_back(passData);

		obj->_secondaryCommandBuffer.Initialize(&info);
	}

	void DrawableBehaviour::RecordFunc(::Rendering::SecondaryCommandBuffer::RecordContext context, VkCommandBuffer commandBuffer)
	{
		DrawableObject* obj = reinterpret_cast<DrawableObject*>(context);
		JE_Assert(obj);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj->PropMaterial->GetPipeline()->GetVkPipeline());	// I guess it'll have to be like that for now...

		VkDescriptorSet descriptorSets[] = { obj->PropMaterial->GetDescriptorSet()->GetVkDescriptorSet() };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj->PropMaterial->GetPipeline()->GetVkPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)obj->_adjVertexBufferArray.size(), obj->_adjVertexBufferArray.data(), obj->_adjOffsetArray.data());
		vkCmdBindIndexBuffer(commandBuffer, obj->PropMesh->GetIndexBuffer(), 0, JE_IndexTypeVk);
		vkCmdDrawIndexed(commandBuffer, obj->PropMesh->GetIndexCount(), 1, 0, 0, 0);
	}

#if !defined(NDEBUG)
	void DrawableBehaviour::CheckObject(const SystemObject* obj)
	{
		JE_Assert(obj != nullptr);
		JE_Assert(ObjectCast(obj)->PropMesh != nullptr);
		JE_Assert(ObjectCast(obj)->PropMaterial != nullptr);
	}
#endif
}
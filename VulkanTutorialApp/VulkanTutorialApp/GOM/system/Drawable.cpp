#include "Drawable.h"

#include "GOM/Entity.h"
#include "Transform.h"

#include "Rendering/resource/Mesh.h"
#include "Rendering/resource/Material.h"
#include "Rendering/resource/buffer/UniformBuffer.h"
#include "Rendering/descriptor/DescriptorSet.h"
#include "Rendering/descriptor/ManagerDescriptor.h"

#include "Core/HelloTriangle.h"
#include "Rendering/Helper.h"

namespace GOM
{
	void DrawableBehaviour::Update()
	{
		for (Component* objAbstract : _componentsAll)
		{
			Drawable* obj = ComponentCast(objAbstract);

			obj->PropMaterial.Get()->Update();
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
		for (Component* objAbstract : _componentsAll)
		{
			Drawable* obj = ComponentCast(objAbstract);
			JE_Assert(obj);

			secondaries.push_back(obj->_secondaryCommandBuffer.GetVkCommandBuffer(currentRenderPass, currentSubpass));
		}

		if (secondaries.size() > 0)
		{
			JE_Assert(secondaries.size() < std::numeric_limits<uint32_t>::max());
			vkCmdExecuteCommands(cmd, (uint32_t)secondaries.size(), secondaries.data());
		}
	}

	Component * DrawableBehaviour::ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam)
	{
		return new Drawable();
	}

	void DrawableBehaviour::InitializeComponent_Internal(Component * objAbstract)
	{
		Drawable* obj = ComponentCast(objAbstract);
		
		AssignPlaceholdersIfNecessary(obj);
		AdjustBuffersForVertexDeclaration(obj);
		CreateDescriptorSet(obj);
		CreateSecondaryCommandBuffer(obj);
	}

	void DrawableBehaviour::CleanupComponent_Internal(Component * objAbstract)
	{
		Drawable* obj = ComponentCast(objAbstract);
		
		obj->_adjVertexBufferArray.clear();
		obj->_adjOffsetArray.clear();

		obj->_secondaryCommandBuffer.Cleanup();

		obj->PropMaterial = nullptr;
		obj->PropMesh = nullptr;
	}

	void DrawableBehaviour::CloneComponent_Internal(Component * destinationAbstract, const Component * sourceAbstract)
	{
		Drawable* destination = ComponentCast(destinationAbstract);
		const Drawable* source = ComponentCast(sourceAbstract);

		destination->PropMesh = source->PropMesh;
		destination->PropMaterial = source->PropMaterial;

		destination->_adjVertexBufferArray = source->_adjVertexBufferArray;
		destination->_adjOffsetArray = source->_adjOffsetArray;
		destination->_secondaryCommandBuffer = source->_secondaryCommandBuffer;
	}

	void DrawableBehaviour::OnSwapChainResize_Internal(Component * objAbstract)
	{
		Drawable* obj = ComponentCast(objAbstract);
		obj->_secondaryCommandBuffer.Reinitialize();
	}

	void DrawableBehaviour::AssignPlaceholdersIfNecessary(Drawable* obj)
	{
		if (obj->PropMaterial.Get() == nullptr)
		{
			JE_SetPropertyPtr(obj, PropMaterial, JE_GetApp()->GetResourceManager()->CacheMaterials.Get("TutorialMaterial"));
		}

		if (obj->PropMesh.Get() == nullptr)
		{
			JE_SetPropertyPtr(obj, PropMesh, JE_GetApp()->GetResourceManager()->CacheMeshes.Get("AutoGen_Box"));
		}
	}

	void DrawableBehaviour::AdjustBuffersForVertexDeclaration(Drawable * obj)
	{
		using namespace Rendering;

		const VertexDeclaration* declaration = obj->PropMaterial.Get()->GetVertexDeclaration();
		const std::vector<VkBuffer>* vertexBufferArray = obj->PropMesh.Get()->GetVertexBuffers();
		const std::vector<VkDeviceSize>* offsetArray = obj->PropMesh.Get()->GetVertexBufferOffsets();
		const uint32_t bufferNum = obj->PropMesh.Get()->GetVertexBufferCount();
		const Mesh::Info* meshInfo = obj->PropMesh.Get()->GetInfo();

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

	void DrawableBehaviour::CreateSecondaryCommandBuffer(Drawable * obj)
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

	void DrawableBehaviour::CreateDescriptorSet(Drawable* obj)
	{
		Entity* myOwner = obj->GetOwner();
		Transform* myTransform = myOwner->GetComponent();
		Rendering::Material* myMaterial = obj->PropMaterial.Get();

		Rendering::DescriptorSet::Info info;
		info.LayInfo = obj->PropMaterial.Get()->GetDescriptorLayout()->Info;

		// TODO: This is a bit hard-coded, but should suffice for now.
		// In future, LayoutInfo *must* specify the exact buffer or texture it wants, otherwise we can only guess...
		for (size_t i = 0; i < Rendering::DescriptorCommon::MAX_BINDINGS_PER_LAYOUT; ++i)
		{
			const Rendering::DescriptorCommon::LayoutInfo::Binding& binding = info.LayInfo.Bindings[i];

			if(binding.Stage == 0)	// Invalid.
				continue;

			if ((Rendering::DescriptorCommon::ShaderStage)binding.Stage == Rendering::DescriptorCommon::ShaderStage::Vertex)
			{
				if (binding.Index == 0)	// Transform goes here.
				{
					const Transform::Data& transformData = myTransform->GetData();
					info.Resources[i][binding.Index] = transformData.UboTransform;
				}
				else
				{
					JE_TODO();
				}
			}
			else if ((Rendering::DescriptorCommon::ShaderStage)binding.Stage == Rendering::DescriptorCommon::ShaderStage::Fragment)
			{
				if (binding.Index == 1)		// Lighting Globals go here.
				{
					info.Resources[i][0] = JE_GetApp()->GetWorld()->GetGlobalParameters()->GetUboLighting();
				}
				else if(binding.Index == 2)	// Drawable textures go here...
				{
					for (size_t j = 0; j < binding.Count; ++j)
					{
						info.Resources[i][j] = myMaterial->GetTexture(j);
					}
				}
				else
				{
					JE_TODO();
				}
			}
			else
			{
				JE_TODO();
			}
		}

		obj->_descriptorSet = JE_GetRenderer()->GetManagerDescriptor()->Get(&info);
	}

	void DrawableBehaviour::RecordFunc(::Rendering::SecondaryCommandBuffer::RecordContext context, VkCommandBuffer commandBuffer)
	{
		Drawable* obj = reinterpret_cast<Drawable*>(context);
		JE_Assert(obj);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj->PropMaterial.Get()->GetPipeline()->GetVkPipeline());	
		// TODO: Test whether this approach will be faster, or re-recording every command but with pipeline bound only once.

		VkDescriptorSet descriptorSets[] = { obj->_descriptorSet->GetVkDescriptorSet() };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj->PropMaterial.Get()->GetPipeline()->GetVkPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)obj->_adjVertexBufferArray.size(), obj->_adjVertexBufferArray.data(), obj->_adjOffsetArray.data());
		vkCmdBindIndexBuffer(commandBuffer, obj->PropMesh.Get()->GetIndexBuffer(), 0, JE_IndexTypeVk);
		vkCmdDrawIndexed(commandBuffer, obj->PropMesh.Get()->GetIndexCount(), 1, 0, 0, 0);
	}

#if JE_BEHAVIOUR_CHECK_COMPONENT
	void DrawableBehaviour::CheckComponent(const Component* obj)
	{
		JE_Assert(obj != nullptr);
		JE_Assert(ComponentCast(obj)->PropMesh.Get() != nullptr);
		JE_Assert(ComponentCast(obj)->PropMaterial.Get() != nullptr);
	}
#endif
}
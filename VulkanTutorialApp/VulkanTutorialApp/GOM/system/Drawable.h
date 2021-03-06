#pragma once

#include "System.h"
#include "Rendering/command/SecondaryCommandBuffer.h"

namespace Rendering
{
	class Mesh;
	class Material;
	class DescriptorSet;
}

namespace GOM
{
	class DrawableBehaviour;

	class Drawable : public Component
	{
		JE_System_Component_Body_Declaration(DrawableBehaviour, Drawable)

	public:

		Util::Property<Rendering::Mesh*> PropMesh;
		Util::Property<Rendering::Material*> PropMaterial;

	protected:

		Drawable()
			: Component()
			, PropMesh(nullptr)
			, PropMaterial(nullptr)
			, _descriptorSet(nullptr)
		{
		}
		virtual ~Drawable()
		{
		}

		std::vector<VkBuffer> _adjVertexBufferArray;
		std::vector<VkDeviceSize> _adjOffsetArray;

		::Rendering::SecondaryCommandBuffer _secondaryCommandBuffer;

		::Rendering::DescriptorSet* _descriptorSet;
	};

	class DrawableBehaviour : public Behaviour
	{
		JE_System_Behaviour_Body_Declaration(DrawableBehaviour, Drawable)

	public:

		// Inherited via Behaviour
		virtual void Update() override;
		virtual void Draw() override;

	protected:

		virtual Component * ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam) override;
		virtual void InitializeComponent_Internal(Component * obj) override;
		virtual void CleanupComponent_Internal(Component * obj) override;
		virtual void CloneComponent_Internal(Component* destination, const Component* source) override;
		JE_System_Behaviour_CheckComponentOverride;

		virtual void OnSwapChainResize_Internal(Component* obj) override;

		void AssignPlaceholdersIfNecessary(Drawable* obj);
		void AdjustBuffersForVertexDeclaration(Drawable* obj);
		void CreateSecondaryCommandBuffer(Drawable* obj);
		void CreateDescriptorSet(Drawable* obj);
		static void RecordFunc(::Rendering::SecondaryCommandBuffer::RecordContext context, VkCommandBuffer commandBuffer);
	};
}
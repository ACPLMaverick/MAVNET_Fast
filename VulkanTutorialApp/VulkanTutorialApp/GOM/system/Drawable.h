#pragma once

#include "System.h"
#include "Rendering/command/SecondaryCommandBuffer.h"

namespace Rendering
{
	class Mesh;
	class Material;
}

namespace GOM
{
	class DrawableBehaviour;

	class DrawableObject : public SystemObject
	{
	public:

		::Rendering::Mesh* PropMesh = nullptr;
		::Rendering::Material* PropMaterial = nullptr;

	protected:

		std::vector<VkBuffer> _adjVertexBufferArray;
		std::vector<VkDeviceSize> _adjOffsetArray;

		::Rendering::SecondaryCommandBuffer _secondaryCommandBuffer;

		friend class DrawableBehaviour;
	};

	class DrawableBehaviour : public SystemBehaviour
	{
		JE_System_Behaviour_Body_Declaration(DrawableBehaviour, DrawableObject)

	public:

		// Inherited via SystemBehaviour
		virtual void Update() override;
		virtual void Draw() override;

	protected:

		virtual SystemObject * ConstructObject_Internal() override;
		virtual void InitializeObject_Internal(SystemObject * obj) override;
		virtual void CleanupObject_Internal(SystemObject * obj) override;
		virtual void CloneObject_Internal(SystemObject* destination, const SystemObject* source) override;
		JE_System_Behaviour_CheckObjectOverride;

		void AdjustBuffersForVertexDeclaration(DrawableObject* obj);
		void CreateSecondaryCommandBuffer(DrawableObject* obj);
		static void RecordFunc(::Rendering::SecondaryCommandBuffer::RecordContext context, VkCommandBuffer commandBuffer);
	};
}
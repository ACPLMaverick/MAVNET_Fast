#pragma once

#include "System.h"

#include <Rendering/resource/Mesh.h>
#include <Rendering/resource/Material.h>

namespace GOM
{
	class DrawableBehaviour;

	class DrawableObject : public SystemDataObject
	{
	public:

		::Rendering::Mesh* PropMesh;
		::Rendering::Material* PropMaterial;

	protected:
		friend class DrawableBehaviour;
	};

	class DrawableBehaviour : public SystemBehaviour
	{
		JE_System_Behaviour_Body_Declaration(DrawableBehaviour, DrawableObject)

	public:

		// Inherited via SystemBehaviour
		virtual void Update() override;
		virtual void Draw() override;
		virtual SystemDataObject * ConstructObject_Internal() override;
		virtual void InitializeObject_Internal(SystemDataObject * obj) override;
		virtual void CleanupObject_Internal(SystemDataObject * obj) override;
		JE_System_Behaviour_CheckObjectOverride;
	};
}
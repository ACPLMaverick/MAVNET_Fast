#pragma once

#include "System.h"

namespace GOM
{
	class RotatorBehaviour;

	class Rotator : public Component
	{
		JE_System_Component_Body_Declaration(RotatorBehaviour, Rotator)

	public:

		Util::Property<glm::vec3> PropRotation;

	protected:

		Rotator()
			: Component()
			, PropRotation(glm::vec3(0.0f, 0.0f, 0.0f))
		{
		}

		virtual ~Rotator()
		{
		}
	};

	class RotatorBehaviour : public Behaviour
	{
		JE_System_Behaviour_Body_Declaration(RotatorBehaviour, Rotator)

	public:

		// Inherited via Behaviour
		virtual void Update() override;
		virtual void Draw() override;

	protected:

		virtual Component * ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam) override;
		virtual void InitializeComponent_Internal(Component * obj) override;
		virtual void CleanupComponent_Internal(Component * obj) override;
		virtual void CloneComponent_Internal(Component* destination, const Component* source) override;
	};
}
#include "Rotator.h"

#include "GOM/Entity.h"
#include "Transform.h"

namespace GOM
{

	void RotatorBehaviour::Update()
	{
		for (Component* componentAbstract : _componentsAll)
		{
			Rotator* rotator = ComponentCast(componentAbstract);
			const glm::vec3& rotationAdd = rotator->PropRotation.Get();

			if (rotationAdd.x == 0.0f && rotationAdd.y == 0.0f && rotationAdd.z == 0.0f)
			{
				continue;
			}

			JE_Assert(rotator->GetOwner());
			Transform* transform = rotator->GetOwner()->GetComponent();
			JE_Assert(transform);

			if (transform->PropMovability.Get() == Transform::Movability::Static)
			{
				continue;
			}

			// TODO: Add delta time.
			transform->AddRotation(rotationAdd * 1.0f);
		}
	}

	void RotatorBehaviour::Draw()
	{
		// Does not get drawn.
	}

	GOM::Component * RotatorBehaviour::ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam)
	{
		return new Rotator();
	}

	void RotatorBehaviour::InitializeComponent_Internal(Component * objAbstract)
	{
		Rotator* rotator = ComponentCast(objAbstract);

		JE_Assert(rotator->GetOwner());
		Transform* transform = rotator->GetOwner()->GetComponent();
		JE_Assert(transform);

		if (transform->PropMovability.Get() == Transform::Movability::Static)
		{
			JE_PrintWarnLine("Rotator component added to Entity with a static Transform.");
		}
	}

	void RotatorBehaviour::CleanupComponent_Internal(Component * objAbstract)
	{
		// Do nothing.
	}

	void RotatorBehaviour::CloneComponent_Internal(Component* destinationAbstract, const Component* sourceAbstract)
	{
		Rotator* destination = ComponentCast(destinationAbstract);
		const Rotator* source = ComponentCast(sourceAbstract);

		JE_SetPropertyPtr(destination, PropRotation, source->PropRotation.Get());
	}

}
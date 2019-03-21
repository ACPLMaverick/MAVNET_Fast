#include "Transform.h"

namespace GOM
{
	void Transform::OnMovabilityChanged()
	{
		JE_PrintLine("Changed!");
	}

	void TransformBehaviour::Update()
	{
		// Regenerate matrices for dynamic objects.
	}

	void TransformBehaviour::Draw()
	{
		// Does not get drawn.
	}

	Component * TransformBehaviour::ConstructObject_Internal()
	{
		return new Transform();
	}

	void TransformBehaviour::InitializeObject_Internal(Component * objAbstract)
	{
	}

	void TransformBehaviour::CleanupObject_Internal(Component * objAbstract)
	{
	}

	void TransformBehaviour::CloneObject_Internal(Component * destinationAbstract, const Component * sourceAbstract)
	{
	}

#if JE_BEHAVIOUR_CHECK_OBJECT
	void TransformBehaviour::CheckObject(const GOM::Component *objAbstract)
	{
	}
#endif
}
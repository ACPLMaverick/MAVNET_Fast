#include "Entity.h"

#include "system/Transform.h"
#include "system/Drawable.h"

namespace GOM
{
	Entity::Entity()
		: _transform(nullptr)
	{
	}


	Entity::~Entity()
	{
		JE_Assert(!_transform);
	}

	void Entity::Initialize()
	{
		
	}

	void Entity::Cleanup()
	{
		if (_transform)
		{
			Transform::GetBehaviour()->CleanupComponent(_transform);
			_transform = nullptr;
		}

		for (Component* component : _components)
		{
			// TODO: This will also need some kind of reflection.
			Drawable* drawablePtr;

			if ((drawablePtr = dynamic_cast<Drawable*>(component)))
			{
				Drawable::GetBehaviour()->CleanupComponent(drawablePtr);
			}
			else
			{
				JE_Assert(false);	// Unimplemented case!
			}
		}
	}
}
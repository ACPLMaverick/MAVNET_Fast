#include "Entity.h"

#include "system/Transform.h"
#include "system/Drawable.h"

#include "Core/HelloTriangle.h"

namespace GOM
{
	Entity::Entity()
		: _transform(nullptr)
	{
	}


	Entity::~Entity()
	{
	}

	void Entity::Initialize()
	{
		_uid = JE_GetApp()->GetManagerUid()->UidCacheEntities.Get();
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
			component->GetMyBehaviour()->CleanupComponent(component);
		}
	}

	bool Entity::operator==(const Entity& other) const
	{
		return _uid == other._uid;
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return _uid != other._uid;
	}

}
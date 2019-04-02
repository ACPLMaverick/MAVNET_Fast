#pragma once

#include "system/System.h"

namespace GOM
{
	class Transform;
	class World;

	class Entity
	{
	public:

		Entity();
		virtual ~Entity();

		// Does nothing atm.
		void Initialize();

		// Destroys all components it owns.
		void Cleanup();


		template<class ComponentType> void AddComponent(ComponentType* component)
		{
			_components.push_back(component);
			component->_owner = this;
		}

		void AddComponent(Transform* component)
		{
			JE_Assert(!_transform);
			_transform = component;
			reinterpret_cast<Component*>(component)->_owner = this;
		}


		template<class ComponentType> void RemoveComponent(ComponentType* component)
		{
			for (std::vector<Component*>::iterator it = _components.begin(); it != _components.end(); ++it)
			{
				if (*it == component)
				{
					_components.erase(it);
					break;
				}
			}
		}

		void RemoveComponent(Transform* component)
		{
			JE_Assert(_transform);
			if (component == _transform)
			{
				_transform = nullptr;
			}
		}


		// Returns first found component of given type.
		template<class ComponentType> ComponentType* GetComponent()
		{
			// TODO: Some kind of a simple reflection.
			return nullptr;
		}

		Transform* GetComponent()
		{
			return _transform;
		}

		// Omits transform on purpose.
		template<class ComponentType> void GetComponents(std::vector<Component*> outComponents)
		{
			// TODO
		}


		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;
		Entity& operator=(const Entity& other) = default;

	private:

		std::vector<Component*> _components;
		Transform* _transform;

		UidEntity _uid;
	};
}
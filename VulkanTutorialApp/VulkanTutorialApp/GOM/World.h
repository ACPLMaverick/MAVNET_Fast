#pragma once

#include "Entity.h"

namespace GOM
{
	class World
	{
	public:

		World();
		~World();

		void Initialize();
		void Cleanup();

		Entity* AddEntity();
		void RemoveEntity(Entity* entity);

		bool HasEntity(Entity* entity) const;

	private:

		typedef std::vector<::GOM::Entity> EntityCollection;

		EntityCollection _entities;
	};
}
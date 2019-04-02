#pragma once

#include "Entity.h"
#include "GlobalWorldParameters.h"

namespace GOM
{
	class World
	{
	public:

		World();
		~World();

		void Initialize();
		void Cleanup();
		void Update();

		Entity* AddEntity();
		void RemoveEntity(Entity* entity);

		bool HasEntity(Entity* entity) const;


		GlobalWorldParameters* GetGlobalParameters() { return &_globalParams; }

	private:

		typedef std::vector<::GOM::Entity> EntityCollection;

		EntityCollection _entities;
		GlobalWorldParameters _globalParams;
	};
}
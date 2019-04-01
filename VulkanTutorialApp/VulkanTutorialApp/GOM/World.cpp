#include "World.h"

namespace GOM
{

	World::World()
	{

	}

	World::~World()
	{

	}

	void World::Initialize()
	{

	}

	void World::Cleanup()
	{
		for (Entity& entity : _entities)
		{
			entity.Cleanup();
		}
		_entities.clear();
	}

	Entity* World::AddEntity()
	{
		_entities.emplace_back();

		Entity* entity = &_entities.back();
		entity->Initialize();

		return entity;
	}

	void World::RemoveEntity(Entity* entity)
	{
		JE_Assert(entity);

		EntityCollection::iterator it = std::find(_entities.begin(), _entities.end(), *entity);
		JE_Assert(it != _entities.end());

		(*it).Cleanup();

		_entities.erase(it);
	}

	bool World::HasEntity(Entity* entity) const
	{
		JE_Assert(entity);
		return std::find(_entities.begin(), _entities.end(), *entity) != _entities.end();
	}
}